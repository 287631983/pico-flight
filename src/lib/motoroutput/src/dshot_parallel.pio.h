// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// -------------- //
// dshot_parallel //
// -------------- //

#define dshot_parallel_wrap_target 0
#define dshot_parallel_wrap 3

#define dshot_parallel_T1 2
#define dshot_parallel_T2 5
#define dshot_parallel_T3 3

static const uint16_t dshot_parallel_program_instructions[] = {
            //     .wrap_target
    0x6028, //  0: out    x, 8                       
    0xa10b, //  1: mov    pins, !null            [1] 
    0xa401, //  2: mov    pins, x                [4] 
    0xa103, //  3: mov    pins, null             [1] 
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program dshot_parallel_program = {
    .instructions = dshot_parallel_program_instructions,
    .length = 4,
    .origin = -1,
};

static inline pio_sm_config dshot_parallel_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + dshot_parallel_wrap_target, offset + dshot_parallel_wrap);
    return c;
}

#include "hardware/clocks.h"
static inline void dshot_parallel_program_init(PIO pio, uint sm, uint offset, uint pin_base, uint pin_count, float freq) {
    for(uint i=pin_base; i<pin_base+pin_count; i++) {
        pio_gpio_init(pio, i);
    }
    pio_sm_set_consecutive_pindirs(pio, sm, pin_base, pin_count, true);
    pio_sm_config c = dshot_parallel_program_get_default_config(offset);
    sm_config_set_out_shift(&c, true, true, 8);
    sm_config_set_out_pins(&c, pin_base, pin_count);
    sm_config_set_set_pins(&c, pin_base, pin_count);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    int cycles_per_bit = dshot_parallel_T1 + dshot_parallel_T2 + dshot_parallel_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

#endif

