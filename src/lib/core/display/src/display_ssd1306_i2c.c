#include <string.h>
#include "ssd1366.h"
#include "font8x8_basic.h"

#include "display_controller.h"

static DisplayControllerConfig_t *config;
static DisplayControllerState_t state;


#define SDA_PIN GPIO_NUM_4
#define SCL_PIN GPIO_NUM_15

#define tag "SSD1306"

void i2c_master_init()
{
	// i2c_config_t i2c_config = {
	// 	.mode = I2C_MODE_MASTER,
	// 	.sda_io_num = SDA_PIN,
	// 	.scl_io_num = SCL_PIN,
	// 	.sda_pullup_en = GPIO_PULLUP_ENABLE,
	// 	.scl_pullup_en = GPIO_PULLUP_ENABLE,
	// 	.master.clk_speed = 1000000
	// };
	// ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_config));
	// ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, i2c_config.mode, 0, 0, 0));
}

void ssd1306_init() {
	// esp_err_t espRc;

	// i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	// i2c_master_start(cmd);
	// i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	// i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

	// i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_OFF, true);
	// i2c_master_write_byte(cmd, OLED_CMD_SET_CHARGE_PUMP, true);
	// i2c_master_write_byte(cmd, 0x14, true);

	// i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP, true); // reverse left-right mapping
	// i2c_master_write_byte(cmd, OLED_CMD_SET_COM_SCAN_MODE, true); // reverse up-bottom mapping

	// i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_ON, true);
	// i2c_master_stop(cmd);

	// espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_RATE_MS);
	// if (espRc == ESP_OK) 
	// 	ESP_LOGI(tag, "OLED configured successfully");
	// else 
	// 	ESP_LOGE(tag, "OLED configuration failed. code: 0x%.2X", espRc);

	// i2c_cmd_link_delete(cmd);
}

// void task_ssd1306_display_pattern(void *ignore) {
// 	i2c_cmd_handle_t cmd;

// 	for (uint8_t i = 0; i < 8; i++) {
// 		cmd = i2c_cmd_link_create();
// 		i2c_master_start(cmd);
// 		i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
// 		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
// 		i2c_master_write_byte(cmd, 0xB0 | i, true);
// 		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
// 		for (uint8_t j = 0; j < 128; j++) {
// 			i2c_master_write_byte(cmd, 0xFF >> (j % 8), true);
// 		}
// 		i2c_master_stop(cmd);
// 		i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
// 		i2c_cmd_link_delete(cmd);
// 	}

// 	vTaskDelete(NULL);
// }


void task_ssd1306_contrast(void *ignore) {
	// i2c_cmd_handle_t cmd;

	// uint8_t contrast = 0;
	// uint8_t direction = 1;
	// while (true) {
	// 	cmd = i2c_cmd_link_create();
	// 	i2c_master_start(cmd);
	// 	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	// 	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
	// 	i2c_master_write_byte(cmd, OLED_CMD_SET_CONTRAST, true);
	// 	i2c_master_write_byte(cmd, contrast, true);
	// 	i2c_master_stop(cmd);
	// 	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	// 	i2c_cmd_link_delete(cmd);
	// 	vTaskDelay(1/portTICK_PERIOD_MS);

	// 	contrast += direction;
	// 	if (contrast == 0xFF) { direction = -1; }
	// 	if (contrast == 0x0) { direction = 1; }
	// }
	// vTaskDelete(NULL);
}

void task_ssd1306_scroll(void *ignore) {
	// esp_err_t espRc;

	// i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	// i2c_master_start(cmd);

	// i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	// i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

	// i2c_master_write_byte(cmd, 0x29, true); // vertical and horizontal scroll (p29)
	// i2c_master_write_byte(cmd, 0x00, true);
	// i2c_master_write_byte(cmd, 0x00, true);
	// i2c_master_write_byte(cmd, 0x07, true);
	// i2c_master_write_byte(cmd, 0x01, true);
	// i2c_master_write_byte(cmd, 0x3F, true);

	// i2c_master_write_byte(cmd, 0xA3, true); // set vertical scroll area (p30)
	// i2c_master_write_byte(cmd, 0x20, true);
	// i2c_master_write_byte(cmd, 0x40, true);

	// i2c_master_write_byte(cmd, 0x2F, true); // activate scroll (p29)

	// i2c_master_stop(cmd);
	// espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	// if (espRc == ESP_OK) {
	// 	ESP_LOGI(tag, "Scroll command succeeded");
	// } else {
	// 	ESP_LOGE(tag, "Scroll command failed. code: 0x%.2X", espRc);
	// }

	// i2c_cmd_link_delete(cmd);

	// vTaskDelete(NULL);
}

void task_ssd1306_display_text(const void *arg_text) {
	// char *text = (char*)arg_text;
	// uint8_t text_len = strlen(text);

	// i2c_cmd_handle_t cmd;

	// uint8_t cur_page = 0;

	// cmd = i2c_cmd_link_create();
	// i2c_master_start(cmd);
	// i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	// i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
	// i2c_master_write_byte(cmd, 0x00, true); // reset column
	// i2c_master_write_byte(cmd, 0x10, true);
	// i2c_master_write_byte(cmd, 0xB0 | cur_page, true); // reset page

	// i2c_master_stop(cmd);
	// i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	// i2c_cmd_link_delete(cmd);

	// for (uint8_t i = 0; i < text_len; i++) {
	// 	if (text[i] == '\n') {
	// 		cmd = i2c_cmd_link_create();
	// 		i2c_master_start(cmd);
	// 		i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	// 		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
	// 		i2c_master_write_byte(cmd, 0x00, true); // reset column
	// 		i2c_master_write_byte(cmd, 0x10, true);
	// 		i2c_master_write_byte(cmd, 0xB0 | ++cur_page, true); // increment page

	// 		i2c_master_stop(cmd);
	// 		i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	// 		i2c_cmd_link_delete(cmd);
	// 	} else {
	// 		cmd = i2c_cmd_link_create();
	// 		i2c_master_start(cmd);
	// 		i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	// 		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
	// 		i2c_master_write(cmd, font8x8_basic_tr[(uint8_t)text[i]], 8, true);

	// 		i2c_master_stop(cmd);
	// 		i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	// 		i2c_cmd_link_delete(cmd);
	// 	}
	// }

	// vTaskDelete(NULL);
}







void displayClear()
{
	//printf("displayClear");
    memset(state.frameBuffer.buffer, 0, sizeof(state.frameBuffer.buffer));
}

#define ON_PIXEL 0x01
#define OFF_PIXEL 0x00

void displayDrawTri(int x0, int y0, int x1, int y1, int x2, int y2, int pixel )
{
	printf("tri: %d, %d, %d, %d, %d, %d\n", x0, y0, x1, y1, x2, y2);
	displayDrawLine(x0, y0, x1, y1, 1);
	displayDrawLine(x1, y1, x2, y2, 1);
	displayDrawLine(x1, y1, x0, y0, 1);
}

void displayDrawLine(int sx, int sy, int ex, int ey, int pixel)
{
	if( sx < 0 || sx >= DISPLAY_WIDTH)
      return;

    if( sy < 0 || sy >= DISPLAY_HEIGHT)
      return;

	if( ex < 0 || ex >= DISPLAY_WIDTH)
      return;

    if( ey < 0 || ey >= DISPLAY_HEIGHT)
      return;

	int x = sx, y = sy;
	int deX = ex;
    float dx = ex - sx;
	
	if( dx < 0 )
	{
		dx = sx - ex;
		x = ex;
		deX = sx;
	}

    float dy = ey - sy;
	if( dy < 0 )
	{
		dy = sy - ey;
		y = ey;
	}

	float ratio = dx != 0.0f ? dy/dx : 0.0f;
    float error = -0.5f;

    while(x < deX)
    {
        if( error > 0)
        {
            ++y;
            error -= 1;
        }
        displayDraw(x, y, pixel);
        ++x;
        error += ratio;
    }
}

void displayDraw(int x, int y, int pixel)
{
    uint16_t pos = ((uint16_t)x) + (y >> 3) * DISPLAY_WIDTH ;

    if(pixel == OFF_PIXEL)
        state.frameBuffer.buffer[pos] &= ~(ON_PIXEL << (y%8));
    else
        state.frameBuffer.buffer[pos] |= ON_PIXEL << (y%8);
}

void displayUpdate()
{
    // i2c_cmd_handle_t cmd;

	// for (uint8_t i = 0; i < 8; i++) 
	// {
	// 	cmd = i2c_cmd_link_create();
	// 	i2c_master_start(cmd);
	// 	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	// 	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
	// 	i2c_master_write_byte(cmd, 0xB0 | i, true);

	// 	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
	// 	i2c_master_write(cmd, &state.frameBuffer.buffer[128*i], 128, true);
	// 	i2c_master_stop(cmd);
	// 	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	// 	i2c_cmd_link_delete(cmd);
	// }
}

void displayInit(DisplayControllerConfig_t *info)
{
    config = info;

// set gpio pin 16 to high to enable screen (heltec board)
    // gpio_config_t io_conf;
    // //disable interrupt
    // io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    // //set as output mode
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // //bit mask of the pins that you want to set,e.g.GPIO18/19
    // io_conf.pin_bit_mask = GPIO_SEL_16;
    // //disable pull-down mode
    // io_conf.pull_down_en = 0;
    // //disable pull-up mode
    // io_conf.pull_up_en = 1;
    // //configure GPIO with the given settings
    // gpio_config(&io_conf);

    // gpio_set_level(GPIO_NUM_16, 1);

	// vTaskDelay(1000/portTICK_RATE_MS);

	// i2c_master_init();
	// ssd1306_init();
}