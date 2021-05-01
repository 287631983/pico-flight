

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "filters.h"
#include "flight_controller.h"
#include "flight_attitude.h"
#include <sys/time.h>

#include "gyro_spi_mpu9250.h"
#include "motor_mixer.h"
#include "motor_output.h"
#include "telemetry.h"
// #include "timers.h"
#include "pico/stdlib.h"
#include "pico/float.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"

#include "math.h"
// #include "display_controller.h"
// #include "render.h"
// #include "system.h"

#define FC_STATE_BOOT 0
#define FC_STATE_CALIBRATE 25
#define FC_STATE_DISARMED 100
#define FC_STATE_FAILSAFE 200
#define FC_STATE_ARMED 300



typedef struct
{
  float lpfAlpha;
  FlightControllerRuntimeState_t r;
  uint64_t startupMs;
  uint32_t gyroTicksPerControl;
} FlightControllerState_t;

static FlightControllerConfig_t *config;
static FlightControllerState_t state;
//static WebSocketDataHandler_t infoHandler;

static uint64_t get_time(void)
{
  // Reading low latches the high value
  uint32_t lo = timer_hw->timelr;
  uint32_t hi = timer_hw->timehr;
  return ((uint64_t)hi << 32u) | lo;
}

bool flightUpdate(repeating_timer_t* timer);
void flightPrintTask();

static repeating_timer_t timer;
static repeating_timer_t idle_timer;

void gyro_ready_callback(uint8_t gpio, uint32_t events)
{
  // Put the GPIO event(s) that just happened into event_str
  // so we can print it

  flightGyroUpdateTask(0);
}




static volatile uint32_t counter0 = 0;
static volatile uint32_t counter1 = 0;

bool flightIdleTimer(repeating_timer_t* timer)
{
  tdv_fc_core0_counter.v.u32 += ((int32_t)(counter0 - tdv_fc_core0_counter.v.u32)) >> 1;
  tdv_fc_core1_counter.v.u32 += ((int32_t)(counter1 - tdv_fc_core1_counter.v.u32)) >> 1;

  counter0 = 0;
  counter1 = 0;
}

void flightCore0()
{  
    // sample counters every millisecond
  if (!add_repeating_timer_us(-1000000.0 / 100.0, flightIdleTimer, NULL, &idle_timer))
  {
    printf("Failed to add timer\n");
    return;
  }

  // negative timeout means exact delay (rather than delay between callbacks)
  if (!add_repeating_timer_us(-1000000.0 / config->updateIntervals.print, flightUpdate, NULL, &timer))
  {
    printf("Failed to add timer\n");
    return;
  }

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  bool status = false;
  
  while(true)
  {
    ++counter0;

    if(counter0 % 100 == 0)
    {
      status = !status;
      gpio_put(PICO_DEFAULT_LED_PIN, status);
    }
  } 
}

void flightCore1()
{
  state.gyroTicksPerControl = (uint32_t)(config->updateIntervals.gyro / config->updateIntervals.control);

  motorMixerInit(&config->mixer);
  motorOutputInit(&config->motorOutputs);
  flightAttitudePIDInit(&config->attitude);
  gyroInit(&config->gyro);

  state.lpfAlpha = lpfAlpha(
      config->gyro.lpfCutoffHz,
      1.0f / config->updateIntervals.gyro);

  printf("gyro delay: %f\n", (float)config->updateIntervals.gyro);
  printf("gyro lpfAlpha: %f\n", state.lpfAlpha);
  printf("gyro scale: %f\n", config->gyro.scale);
  printf("control delay: %f, gyro ticks: %u\n", (float)config->updateIntervals.control, state.gyroTicksPerControl);
  printf("print delay: %f\n", (float)config->updateIntervals.print);

  gyroSetUpdateCallback(flightGyroUpdateTask);

  multicore_fifo_push_blocking(1);

  while(true)
  {
    ++counter1;
  }
}

void flightInit(FlightControllerConfig_t *info)
{
  sleep_ms(2000);

  tdv_fc_state.v.i32 = FC_STATE_BOOT;
  config = info;

  telemetryInit(&config->telemetry);

  multicore_launch_core1(flightCore1);

  // wait for control loop core init to complete
  int core1_init = multicore_fifo_pop_blocking();

  inputRxInit(&config->input);
  osdInit(&config->osd);

  flightInitVars();

  state.startupMs = get_time();

  flightCore0();
}

static inline float constrainf(float amt, float low, float high)
{
  if (amt < low)
    return low;

  if (amt > high)
    return high;

  return amt;
}

#define SETPOINT_RATE_LIMIT 1998.0f
#define RC_RATE_INCREMENTAL 14.54f

float applyBetaflightRates(const int axis, float rcCommandf, const float rcCommandfAbs)
{
  ControlRates_t *rates = &config->control.rates[axis];

  if (rates->expo)
  {
    const float expof = rates->expo / 100.0f;
    rcCommandf = rcCommandf * (rcCommandfAbs * rcCommandfAbs * rcCommandfAbs) * expof + rcCommandf * (1 - expof);
  }

  float rcRate = rates->rc / 100.0f;
  if (rcRate > 2.0f)
    rcRate += RC_RATE_INCREMENTAL * (rcRate - 2.0f);

  float angleRate = 200.0f * rcRate * rcCommandf;
  if (rates->super)
  {
    const float rcSuperfactor = 1.0f / (constrainf(1.0f - (rcCommandfAbs * (rates->super / 100.0f)), 0.01f, 1.00f));
    angleRate *= rcSuperfactor;
  }

  return angleRate;
}

// TODO: need way to fake control inputs...
void flightProcessInputs()
{
  inputRxUpdate();

  ControlState_t *control = inputGetControlState();
  memcpy(&(state.r.controlInputs), control, sizeof(ControlState_t));

  state.r.controlInputs.failsafe = false;
  state.r.controlInputs.signalLost = false;
  for (int a = 0; a < NUM_AXIS_CONTROLS; ++a)
  {
    //float value = state.r.controlInputs.values[a];
    state.r.controllerInputs[a] = state.r.controlInputs.values[a]; //applyBetaflightRates(a, value, fabsf(value));
  }

  // printf("[% 1.3f, % 1.3f, % 1.3f, % 1.3f, % 1.3f, %u]\n",
  //        state.r.controlInputs.values[INPUT_CONTROL_PITCH],
  //        state.r.controlInputs.values[INPUT_CONTROL_ROLL],
  //        state.r.controlInputs.values[INPUT_CONTROL_YAW],
  //        state.r.controlInputs.values[INPUT_CONTROL_TROTTLE],
  //        state.r.controlInputs.values[INPUT_CONTROL_ARM],
  //        state.r.state);

  // state.r.controllerInputs.throttle = state.r.controlInputs.values[3];
  bool8v(tdv_fc_in_armed) = control->values[INPUT_CONTROL_ARM].value > fixed_from_float(0.5f).value;
}

void flightControlUpdate()
{
  // flightProcessInputs();
  state.r.motorOutputs.disarmed = true;
  uint32_t next = tdv_fc_state.v.u32;

  switch (tdv_fc_state.v.u32)
  {
  case FC_STATE_BOOT:
    next = FC_STATE_CALIBRATE;
    break;

  case FC_STATE_CALIBRATE:
  {
    GyroState_t *gs = gyroState();
    if (gs != NULL && (gs->state == GYRO_ST_READY || gs->state == GYRO_ST_FAIL))
      next = FC_STATE_DISARMED;
    break;
  }

  case FC_STATE_ARMED:

    if (state.r.controlInputs.failsafe || state.r.controlInputs.signalLost)
    {
      next = FC_STATE_FAILSAFE;
      break;
    }

    if (!tdv_fc_in_armed.value.b8)
    {
      next = FC_STATE_DISARMED;
      break;
    }

    state.r.motorOutputs.disarmed = false;

    //state.r.motorInputs = state.r.controllerInputs;

    // state.r.motorInputs = flightAttitudeUpdate(
    //     state.r.controllerInputs,
    //     state.r.gyroFilteredRates,
    //     1.0f / config->updateIntervals.control);
    break;

  case FC_STATE_FAILSAFE:
    if (!bool8v(tdv_fc_in_armed) && !bool8v(tdv_fc_in_failsafe) && !bool8v(tdv_fc_in_signalLost))
    {
      next = FC_STATE_DISARMED;
    }
    break;

  case FC_STATE_DISARMED:
    if (bool8v(tdv_fc_in_armed) && !bool8v(tdv_fc_in_failsafe) && !bool8v(tdv_fc_in_signalLost))
    {
      next = FC_STATE_ARMED;
      break;
    }
    break;

  default:
  {
    uint64_t now = (uint64_t)get_time();

    if (now < (state.startupMs + (uint64_t)config->motorOutputs.startupDelayMs))
    {
      //++state.controlUpdates;
    }
    else if (now < (state.startupMs + config->motorOutputs.startupDelayMs * 2))
    {
      for (int m = 0; m < tdv_motor_count.v.u8; ++m)
        tdv_fc_motor_output[m].v.f32 = 0;
    }
    else
    {
      next = FC_STATE_DISARMED;
    }
  }
  break;
  }

  if (next != tdv_fc_state.v.u32)
  {
    printf("state changing: %u -> %u\n", tdv_fc_state.v.u32, next);
    tdv_fc_state.v.u32 = next;
    telemetry_sample(&tdv_fc_state);
  }

  //motorMixerCalculateOutputs(&state.r.motorInputs, &state.r.motorOutputs);
  motorOutputSet(&state.r.motorOutputs);

  ++tdv_fc_control_updates.v.u32;
}

#define GYRO_SCALE (1000.0f / (float)(65535.0f/2.0f))

// TODO: wait for gyro data ready interrupt
void flightGyroUpdateTask()
{
  uint32_t t = timer_hw->timelr;
  GyroState_t* gyro = gyroState();

  tdv_fc_rates_raw[0].v.f32 = ((float)gyro->raw_rates.axis[0]) / 32.8f; //* GYRO_SCALE;
  tdv_fc_rates_raw[1].v.f32 = ((float)gyro->raw_rates.axis[1]) / 32.8f; //* GYRO_SCALE;
  tdv_fc_rates_raw[2].v.f32 = ((float)gyro->raw_rates.axis[2]) / 32.8f; //* GYRO_SCALE;
  
  // TODO: explore using hardware interp in blend mode as a low pass filter

  tdv_fc_rates_filtered[0].v.f32 = lowPassFilter(tdv_fc_rates_filtered[0].v.f32, tdv_fc_rates_raw[0].v.f32, state.lpfAlpha);
  tdv_fc_rates_filtered[1].v.f32 = lowPassFilter(tdv_fc_rates_filtered[1].v.f32, tdv_fc_rates_raw[1].v.f32, state.lpfAlpha);
  tdv_fc_rates_filtered[2].v.f32 = lowPassFilter(tdv_fc_rates_filtered[2].v.f32, tdv_fc_rates_raw[2].v.f32, state.lpfAlpha);

  ++tdv_fc_gyro_updates.v.u32;

  if(tdv_fc_gyro_updates.v.u32 % state.gyroTicksPerControl == 0 )
  {
    flightControlUpdate();
  }

  int32_t dt = timer_hw->timelr - t;
  tdv_fc_gyro_update_us.v.u32 += ((int32_t)(dt - tdv_fc_gyro_update_us.v.u32)) >> 1;
  //return true;
}

bool flightUpdate(repeating_timer_t* timer)
{
  flightPrintTask();
  return true;
}


static int x = 0;
static int y = 0;
static char buffer[64];
char a = 0;
double temp = 0.0f;

void flightPrintTask()
{
  uint64_t time = get_time();

  telemetry_sample_array(tdv_fc_rates_raw, 3);
  telemetry_sample_array(tdv_fc_rates_filtered, 3);

  telemetry_sample_array(tdv_fc_motor_output, tdv_motor_count.v.u8);

  telemetry_sample(&tdv_fc_gyro_updates);
  telemetry_sample(&tdv_fc_state);
  telemetry_sample(&tdv_fc_control_updates);

  telemetry_sample(&tdv_fc_core0_counter);
  telemetry_sample(&tdv_fc_core1_counter);

  telemetry_sample(&tdv_fc_gyro_update_us);

  tdv_fc_core0_counter.v.u32 = 0;
  tdv_fc_core1_counter.v.u32 = 0;

  
  telemetry_send(state.startupMs, time);

  //osdDraw( 40 + (x++ * 8) % 100, (x / 100)+25,1);
  // osdDraw( (x) % 515, 11, (x % 4) > 0);
  // ++x;

  // //printf("buffer=%s\n",buffer);
  // temp = tdv_fc_gyro_roll.value.f32;
  // sprintf(buffer, "%f    ",temp);
  // osdDrawString(10, 80, buffer);

  // sprintf(buffer, "% f    ", tdv_fc_gyro_pitch.value.f32);
  // osdDrawString(10, 90, buffer);

  // sprintf(buffer, "%f    ", tdv_fc_gyro_yaw.value.f32);
  // osdDrawString(10, 100, buffer);
  
  //osdDrawString2(40 + (x += 16) % 200, ((x / 200)*16)%200 +25, buffer);
}
