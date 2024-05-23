/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "st7735.h"
#include "los_task.h"
#include "gpio_if.h"
#include "spi_if.h"
#include "esp_err.h"
#include "gpio_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

#define PIN_NUM_DC   (4)
#define PIN_NUM_RST  (5)
#define PIN_NUM_BCKL (6)
#define PIN_POWER    (2)

static uint16_t ShowBuf[ROW_PIXEL][COL_PIXEL];
static DevHandle SpiHandle = NULL;

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes;
} lcd_init_cmd_t;

static const lcd_init_cmd_t st_init_cmds[] = {
    {0x11, {0}, 0x40},
    {0xB1, {0x01, 0x2C, 0x2D}, 3},
    {0xB2, {0x01, 0x2C, 0x2D}, 3},
    {0xB3, {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}, 6},
    {0xB4, {0x07}, 1},
    {0xC0, {0xA2, 0x02, 0x84}, 3},
    {0xC1, {0xC5}, 1},
    {0xC2, {0x0A, 0x00}, 2},
    {0xC3, {0x8A, 0x2A}, 2},
    {0xC4, {0x8A, 0xEE}, 2},
    {0xC5, {0x0E}, 1},
    {0x36, {0xC8}, 1},
    {0xE0, {0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22, 0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10}, 16},
    {0xE1, {0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10}, 16},
    {0xF0, {0x01}, 1},
    {0xF6, {0x00}, 1},
    {0x3A, {0x05}, 1},
    {0x29, {0}, 0x40}
};

static void LcdCmd(uint8_t cmd)
{
    struct SpiMsg msg = {0};
    msg.wbuf = &cmd;
    msg.len = 1;
    GpioWrite(PIN_NUM_DC, GPIO_VAL_LOW);
    (void)SpiTransfer(SpiHandle, &msg, 1);
    GpioWrite(PIN_NUM_DC, GPIO_VAL_HIGH);
}

static void LcdData(uint8_t *data, int len)
{
    struct SpiMsg msg = {0};
    msg.wbuf = (uint8_t *)data;
    msg.len = len;
    (void)GpioWrite(PIN_NUM_DC, GPIO_VAL_HIGH);
    (void)SpiTransfer(SpiHandle, &msg, 1);
}

static void IomuxInit(void)
{
    gpio_config_t io_conf = {0};
    io_conf.pin_bit_mask = GPIO_SEL_15;
    io_conf.mode = GPIO_MODE_OUTPUT;
    if (gpio_config(&io_conf) != ESP_OK) {
        return;
    }
    (void)gpio_iomux_out(GPIO_NUM_15, FUNC_MTDO_GPIO15, 1);
}

void LcdInit(void)
{
    esp_err_t ret;
    struct SpiDevInfo spiDevinfo = {0};
    spiDevinfo.busNum = HSPI_HOST;
    spiDevinfo.csNum = 0;
    SpiHandle = SpiOpen(&spiDevinfo);
    if (SpiHandle == NULL) {
        return NULL;
    }
    IomuxInit();

    (void)GpioSetDir(PIN_POWER, GPIO_DIR_OUT);
    (void)GpioSetDir(PIN_NUM_RST, GPIO_DIR_OUT);
    (void)GpioSetDir(PIN_NUM_BCKL, GPIO_DIR_OUT);
    (void)GpioSetDir(PIN_NUM_DC, GPIO_DIR_OUT);

    (void)GpioWrite(PIN_POWER, GPIO_VAL_HIGH);
    (void)GpioWrite(PIN_NUM_BCKL, GPIO_VAL_LOW);
    (void)GpioWrite(PIN_NUM_RST, GPIO_VAL_LOW);
    (void)GpioWrite(PIN_NUM_DC, GPIO_VAL_HIGH);

    LOS_Msleep('d');
    (void)GpioWrite(PIN_NUM_RST, GPIO_VAL_HIGH);
    LOS_Msleep('d');
    for (unsigned i = 0; i < (sizeof(st_init_cmds) / sizeof(st_init_cmds[0])); ++i) {
        LcdCmd(st_init_cmds[i].cmd);
        LcdData(st_init_cmds[i].data, st_init_cmds[i].databytes & '?');
        if (st_init_cmds[i].databytes > '?') {
            LOS_Msleep('d');
        }
    }

    (void)memset_s(ShowBuf, sizeof(ShowBuf), 0x00, sizeof(ShowBuf));
    LcdPush();

    GpioWrite(PIN_NUM_BCKL, GPIO_VAL_HIGH);
}

void LcdDeInit(void)
{
    if (SpiHandle) {
        SpiClose(SpiHandle);
        SpiHandle = NULL;
    }
}

void LcdPush(void)
{
    static uint8_t const x_data[4] = {0, 0, 0, COL_PIXEL-1};
    static uint8_t const y_data[4] = {0, 0, 0, ROW_PIXEL-1};
    LcdCmd('*');
    LcdData(x_data, '\4');
    LcdCmd('+');
    LcdData(y_data, '\4');
    LcdCmd(',');
    LcdData(((uint8_t *)ShowBuf), sizeof(ShowBuf));
}

void inline lcd_draw_point(uint8_t x, uint8_t y, uint16_t color)
{
    ShowBuf[y][x] = color;
}
