#include <stdio.h>
#include <hdf_log.h>
#include "pwm_if.h"
#include "cmsis_os2.h"
#include "osal_irq.h"
#include "osal_time.h"
#include "los_task.h"

#define HDF_LOG_TAG         pwm_test

#define HDF_PWM_STACK_SIZE 0x1000
#define HDF_PWM_TASK_NAME "hdf_pwm_test_task"
#define HDF_PWM_TASK_PRIORITY 30

DevHandle pwm_handle = NULL;

static void HdfPwmtTestEntry(void* arg)
{
    uint32_t num = 0;
    uint32_t ret = 0;
    struct PwmConfig config;

    printf("%s start \n", __func__);
    pwm_handle = PwmOpen(num);
    if(pwm_handle == NULL)
    {
        printf("PwmOpen: pwm %u failed!\n", num);
        return;
    }

    //GD32f470系统时钟为200M, 当前APP中可配置PWM的周期最小是1us

    //设置PWM设备参数,设置周期为2，单位：us,占空比为1/2
    config.duty = 1;
    config.period = 2;
    config.polarity = PWM_NORMAL_POLARITY;
    config.status = PWM_ENABLE_STATUS;  //pwm enable: PWM_ENABLE_STATUS, pwm disable: PWM_DISABLE_STATUS

    ret = PwmSetConfig(pwm_handle, &config);
    if(ret != 0) {
        printf("PwmSetConfig: ret %u failed!\n", ret);
        return;
    }

    printf("%s end \n", __func__);

    while (1) {
        printf("TaskSample:PWM task create start!\n");
        osDelay(1000);
    }
}

void StartHdfPwmTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_PWM_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_PWM_STACK_SIZE;
    attr.priority = HDF_PWM_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfPwmtTestEntry, NULL, &attr) == NULL) {
        HDF_LOGE("Failed to create thread1!\n");
    }
}