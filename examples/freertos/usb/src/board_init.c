// Copyright 2021 XMOS LIMITED. This Software is subject to the terms of the
// XMOS Public License: Version 1

#include <platform.h>
#include <xs1.h>
#include <stdarg.h>
#include <xcore/hwtimer.h>

#include "board_init.h"
#include "app_conf.h"

static rtos_driver_rpc_t gpio_rpc_config;

void board_tile0_init(
        chanend_t tile1,
        rtos_intertile_t *intertile1_ctx,
        rtos_qspi_flash_t *qspi_flash_ctx,
        rtos_gpio_t *gpio_ctx)
{
    rtos_intertile_init(intertile1_ctx, tile1);
    rtos_intertile_t *client_intertile_ctx[1] = {intertile1_ctx};

    rtos_gpio_init(
            gpio_ctx);

    rtos_gpio_rpc_host_init(
            gpio_ctx,
            &gpio_rpc_config,
            client_intertile_ctx,
            1);

    rtos_qspi_flash_init(
            qspi_flash_ctx,
            XS1_CLKBLK_2,
            PORT_SQI_CS,
            PORT_SQI_SCLK,
            PORT_SQI_SIO,

            /** Derive QSPI clock from the 700 MHz xcore clock **/
            qspi_io_source_clock_xcore,

            /** Full speed clock configuration **/
            5, // 700 MHz / (2*5) -> 70 MHz,
            1,
            qspi_io_sample_edge_rising,
            0,
            /** SPI read clock configuration **/
            12, // 700 MHz / (2*12) -> ~29 MHz
            0,
            qspi_io_sample_edge_falling,
            0,

            1, /* Enable quad page programming */
            QSPI_IO_BYTE_TO_MOSI(0x38), /* The quad page program command */

            QSPI_IO_BYTE_TO_MOSI(0x05),  /* The quad enable register read command */
            QSPI_IO_BYTE_TO_MOSI(0x01),  /* The quad enable register write command */
            0x40,                        /* quad_enable_bitmask */

            256, /* page size is 256 bytes */
            16384); /* the flash has 16384 pages */
}

void board_tile1_init(
        chanend_t tile0,
        rtos_intertile_t *intertile1_ctx,
        rtos_gpio_t *gpio_ctx)
{
    rtos_intertile_init(intertile1_ctx, tile0);

    rtos_gpio_rpc_client_init(
            gpio_ctx,
            &gpio_rpc_config,
            intertile1_ctx);
}
