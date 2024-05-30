#include <stdio.h>
#include "main.h"
#include "hdf_log.h"
#include "los_task.h"
#include "cmsis_os2.h"
#include "uart_if.h"

#define HDF_UART_STACK_SIZE        0x1000
#define HDF_UART_TASK_NAME         "hdf_uart_test_task"
#define HDF_UART_TASK_PRIORITY     26

uint8_t tx_buffer[] = "this is usart dma demo";
uint8_t rbuff[5] = { 0 };
uint32_t USART_RX_NUM = 0;

static void* HdfUartTestEntry(void* arg)
{
    uint32_t ret = 0;
    DevHandle handle = NULL;
    DevHandle handle2 = NULL;
    uint32_t read_len1 = 0;
    uint32_t read_len2 = 0;
    uint32_t port1 = 1; 
    uint32_t port2 = 2; 

    handle = UartOpen(port1);
    if (handle == NULL) {
        HDF_LOGE("UartOpen port%d: failed!\n", port1);
        return;
    }

    handle2 = UartOpen(port2);
    if (handle2 == NULL) {
        HDF_LOGE("UartOpen port%d: failed!\n", port2);
        return;
    }

    ret = UartWrite(handle, "hello", 5);
    if (ret != 0) {
        HDF_LOGE("UartWrite: failed, ret %d\n", ret);
    }

    ret = UartWrite(handle2, "hello", 5);
    if (ret != 0) {
        HDF_LOGE("UartWrite: failed, ret %d\n", ret);
    }

    /* 从UART设备读取5字节的数据 */
    while(1) {
        LOS_TaskDelay(100);

        read_len1 = UartRead(handle, rbuff, 5);
        if(read_len1 != 0) {
            printf("read_len1 %d\n", read_len1);
            for (int i = 0; i< read_len1; i++) {
                printf("%x", rbuff[i]);
            }
            printf("\n");

            UartWrite(handle, &rbuff, read_len1);
        }

        read_len2 = UartRead(handle2, rbuff, 5);
        if(read_len2 != 0) {
            printf("read_len2 %d\n", read_len2);
            for (int i = 0; i< read_len2; i++) {
                printf("%x", rbuff[i]);
            }
            printf("\n");

            UartWrite(handle2, &rbuff, read_len2);
        }
    }
}

void StartHdfUartTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_UART_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_UART_STACK_SIZE;
    attr.priority = HDF_UART_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfUartTestEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}

