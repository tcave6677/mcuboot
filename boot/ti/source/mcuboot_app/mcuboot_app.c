/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== blinky.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "bootutil/bootutil.h"
#include "bootutil/image.h"

#include "mcuboot_config/mcuboot_logging.h"

#define BLINK_INTERVAL     500000  /* Set blink interval to 500000us or 500ms */

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    /* Toggle an LED */
    GPIO_toggle(CONFIG_GPIO_LED_0);
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_1.
 *  This may not be used for all boards.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn1(uint_least8_t index)
{
    /* Toggle an LED */
    GPIO_toggle(CONFIG_GPIO_LED_1);
}

static void start_app(uint32_t vector_table) {

    static uint32_t temp;
    temp = vector_table;
    // Reset the stack pointer,
    temp +=4;
    __asm volatile (" LDR SP, [R0, #0x0] ");
    ((void (*)(void))(*((uint32_t*)temp)))();
}

static void do_boot(struct boot_rsp *rsp) {
    MCUBOOT_LOG_INF("Starting Main Application");
    MCUBOOT_LOG_INF("  Image Start Offset: 0x%x", (int)rsp->br_image_off);

    // We run from internal flash. Base address of this medium is 0x0
    uint32_t vector_table = 0x0 + rsp->br_image_off + rsp->br_hdr->ih_hdr_size;

    MCUBOOT_LOG_INF("  Vector Table Start Address: 0x%x",
    (int)vector_table);

    start_app(vector_table);
}


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    fih_int bootStatus;
    struct boot_rsp bootRsp;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED and button pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_INPUT | GPIO_CFG_IN_PD);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_INPUT | GPIO_CFG_IN_PD);

    /* Init LED's state */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);

    trace_init();
    MCUBOOT_LOG_INF("mcuboot_app");

    bootStatus = boot_go(&bootRsp);

    MCUBOOT_LOG_INF("bootRsp: status=%x, offset = %x, ver=%d.%d.%d.%d",
                        bootStatus,
                        bootRsp.br_image_off,
                        bootRsp.br_hdr->ih_ver.iv_major,
                        bootRsp.br_hdr->ih_ver.iv_minor,
                        bootRsp.br_hdr->ih_ver.iv_revision,
                        bootRsp.br_hdr->ih_ver.iv_build_num);

    if(FIH_SUCCESS == bootStatus)
    {
        do_boot(&bootRsp);
    }
    else
    {
        while(1)
        {
            usleep(BLINK_INTERVAL);
            GPIO_write(CONFIG_GPIO_LED_0, !GPIO_read(CONFIG_GPIO_LED_0));
        }
    }
}
