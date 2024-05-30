/*
 * Copyright (c) 2021 HiHope Open Source Organization.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "i2c_if.h"
#include "los_task.h"
#include "hdf_log.h"
#include "main.h"
#include "cmsis_os2.h"

#define HDF_I2C_STACK_SIZE        0x1000
#define HDF_I2C_TASK_NAME         "hdf_gpio_test_task"
#define HDF_I2C_TASK_PRIORITY     21

#define AT24C02_IIC_ADDR    0xA0     //EEPROM器件地址
#define I2C_MSG_NUM 2                //I2c传输msgs个数

struct eep24c02 {
    DevHandle i2c;
    uint16_t i2c_id;
    uint16_t i2c_addr;
};

static struct eep24c02 priv = {
    .i2c_id = 0,
    .i2c_addr = AT24C02_IIC_ADDR,
};

uint8_t const Eep_W_ADD[] = {0x00, 0x10};             //写eep地址0x0010，高位在前，低位在后
uint8_t const Eep_W_DATA[8] = {39,7,6,5,4,3,2,50};      //写入数据

uint8_t const Eep_R_ADD[] = {0x00, 0x10};  //写eep地址0x0010，高位在前，低位在后
uint8_t Eep_R_DATA[8] = {0};         //读数据缓存

uint8_t const Eep_W_ADD_2[] = {0x00, 0x18};             //写eep地址0x0018，高位在前，低位在后
uint8_t const Eep_W_DATA_2[8] = {4,2,3,4,5,6,7,11};      //写入数据

uint8_t const Eep_R_ADD_2[] = {0x00, 0x18};  //写eep地址0x0018，高位在前，低位在后
uint8_t Eep_R_DATA_2[8] = {0};         //读数据缓存


//msg_w[1]配置写地址，msg_w[1]配置写数据
struct I2cMsg msg_w[I2C_MSG_NUM] = {
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 0,                //读/写标志0-write 1-read
        .len = sizeof(Eep_W_ADD),  //写地址长度
        .buf = Eep_W_ADD,          //写地址buf
    },
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 0,                //读/写标志0-write 1-read
        .len = sizeof(Eep_W_DATA), //写数据长度
        .buf = Eep_W_DATA,         //写数据buf
    },
};

//msg_r[0]配置读地址，msg_w[1]配置读数据
struct I2cMsg msg_r[I2C_MSG_NUM] = {
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 1,                //读/写标志0-write 1-read
        .len = sizeof(Eep_R_ADD),  //读地址长度
        .buf = Eep_R_ADD,          //读地址buf
    },
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 1,                //读/写标志0-write 1-read
        .len = sizeof(Eep_R_DATA), //读地址长度
        .buf = Eep_R_DATA,         //读地址buf
    },
};

//msg_w_2[1]配置写地址，msg_w_2[1]配置写数据
struct I2cMsg msg_w_2[I2C_MSG_NUM] = {
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 0,                //读/写标志0-write 1-read
        .len = sizeof(Eep_W_ADD_2),  //写地址长度
        .buf = Eep_W_ADD_2,          //写地址buf
    },
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 0,                //读/写标志0-write 1-read
        .len = sizeof(Eep_W_DATA_2), //写数据长度
        .buf = Eep_W_DATA_2,         //写数据buf
    },
};

//msg_r_2[0]配置读地址，msg_w_2[1]配置读数据
struct I2cMsg msg_r_2[I2C_MSG_NUM] = {
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 1,                //读/写标志0-write 1-read
        .len = sizeof(Eep_R_ADD_2),  //读地址长度
        .buf = Eep_R_ADD_2,          //读地址buf
    },
    {
        .addr = AT24C02_IIC_ADDR,  //器件地址
        .flags = 1,                //读/写标志0-write 1-read
        .len = sizeof(Eep_R_DATA_2), //读地址长度
        .buf = Eep_R_DATA_2,         //读地址buf
    },
};

VOID I2cTaskSampleEntry(VOID)
{
    UINT32 ret;

    printf("%s: %d", __func__, __LINE__);

    while (1) {
        priv.i2c = I2cOpen(priv.i2c_id);

        printf("%s: priv.i2c = %d\n", __func__, priv.i2c_id);

        //wirte eep24c02
        ret = I2cTransfer(priv.i2c, &msg_w, I2C_MSG_NUM);
        if (ret != 0) {
            printf("I2cTransfer: failed, ret %u\n", ret);
        }
        (VOID)LOS_TaskDelay(1000);

        //read eep24c02
        ret = I2cTransfer(priv.i2c, &msg_r, I2C_MSG_NUM);
        if (ret != 0) {
            printf("I2cTransfer: failed, ret %u\n", ret);
        }

        //printf read data
        printf("%s: [line = %d] msg data is: \r\n",__func__, __LINE__);
        for(int i = 0; i < 8;i++) {
            printf("msg[%d] = %d, ",i, msg_r[1].buf[i]);
        }
        printf("\r\n");

        //wirte eep24c02
        ret = I2cTransfer(priv.i2c, &msg_w_2, I2C_MSG_NUM);
        if (ret != 0) {
            printf("I2cTransfer: failed, ret %u\n", ret);
        }
        (VOID)LOS_TaskDelay(1000);

        //read eep24c02
        ret = I2cTransfer(priv.i2c, &msg_r_2, I2C_MSG_NUM);
        if (ret != 0) {
            printf("I2cTransfer: failed, ret %u\n", ret);
        }

        //printf read data
        printf("%s: [line = %d] msg2 data is: \r\n",__func__, __LINE__);
        for(int i = 0; i < 8;i++) {
            printf("msg2[%d] = %d, ",i, msg_r_2[1].buf[i]);
        }
        printf("\r\n");

        (VOID)LOS_TaskDelay(2000);
    }
}

void StartHdfI2cTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_I2C_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_I2C_STACK_SIZE;
    attr.priority = HDF_I2C_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)I2cTaskSampleEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}

