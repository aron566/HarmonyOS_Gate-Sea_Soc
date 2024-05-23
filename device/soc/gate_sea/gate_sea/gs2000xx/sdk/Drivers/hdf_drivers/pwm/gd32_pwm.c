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
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "pwm_core.h"
#include "pwm_if.h"
#include "gd32_pwm.h"
#include "hal_gpio.h"

#define DEBUG_PWM_LOG            0

GD32HrPwm *gd32Hrpwm = NULL;

int32_t pwm_timer_disable(struct PwmDev *pwm, struct PwmConfig *config)
{
    GD32HrPwm *pwmDevice = (GD32HrPwm *)pwm->priv;

    timer_disable(g_tmrMapCmd[pwmDevice->timer]);
}

void pwm_gpio_config(struct PwmDev *pwm)
{
    GD32HrPwm *pwmDevice = (GD32HrPwm *)pwm->priv;
#if DEBUG_PWM_LOG
    HDF_LOGI("%s: [line = %d] [ch_num = %d]\n", __func__, __LINE__, pwmDevice->gd32PwmCfg.ch_num);
#endif
    for(int i = 0; i < pwmDevice->gd32PwmCfg.ch_num; i ++) {
#if DEBUG_PWM_LOG
        HDF_LOGI("%s: [line = %d] [channel[%d] = %d]\n", __func__, __LINE__, i, pwmDevice->gd32PwmCfg.channel[i]);
        HDF_LOGI("%s: [line = %d] [port[%d] = %d]\n", __func__, __LINE__, i, pwmDevice->gd32PwmCfg.port[i]);
        HDF_LOGI("%s: [line = %d] [pin[%d] = %d]\n", __func__, __LINE__, i, pwmDevice->gd32PwmCfg.pin[i]);
        HDF_LOGI("%s: [line = %d] [func[%d] = %d]\n", __func__, __LINE__, i, pwmDevice->gd32PwmCfg.func[i]);
#endif
        rcu_periph_clock_enable(g_rcuPeriphMaps[pwmDevice->gd32PwmCfg.port[i]]);
        gpio_mode_set(g_gpioxMaps[pwmDevice->gd32PwmCfg.port[i]], GPIO_MODE_AF, GPIO_PUPD_NONE, g_GdRealPinMaps[pwmDevice->gd32PwmCfg.pin[i]]);
        gpio_output_options_set(g_gpioxMaps[pwmDevice->gd32PwmCfg.port[i]], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, g_GdRealPinMaps[pwmDevice->gd32PwmCfg.pin[i]]);
        gpio_af_set(g_gpioxMaps[pwmDevice->gd32PwmCfg.port[i]], g_gpioAFMap[pwmDevice->gd32PwmCfg.func[i]], g_GdRealPinMaps[pwmDevice->gd32PwmCfg.pin[i]]);
    }
}

void pwm_timer_config(struct PwmDev *pwm, struct PwmConfig *config)
{
    GD32HrPwm *pwmDevice = (GD32HrPwm *)pwm->priv;

#if DEBUG_PWM_LOG
    HDF_LOGI("%s: [line = %d] [period = %d] [duty = %d] [polarity = %d]\n", __func__, __LINE__, config->period, config->duty, config->polarity);
    HDF_LOGI("%s: [line = %d] [timer = %d]\n", __func__, __LINE__, pwmDevice->timer);
#endif

    for(int i = 0; i < pwmDevice->gd32PwmCfg.ch_num; i ++) {
        HDF_LOGI("%s: [line = %d] [channel[%d] = %d]\n", __func__, __LINE__, i, pwmDevice->gd32PwmCfg.channel[i]);
    }

    /* TIMER configuration: generate PWM signals with different duty cycles:
       TIMERCLK = SystemCoreClock / prescaler, prescaler = 200  SystemCoreClock = 200000000 */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(g_rcu_tmrMapCmd[pwmDevice->timer]);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    timer_struct_para_init(&timer_initpara);
    timer_deinit(g_tmrMapCmd[pwmDevice->timer]);

    /* TIMER configuration */
    timer_initpara.prescaler         = 200 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = config->period - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(g_tmrMapCmd[pwmDevice->timer], &timer_initpara);

    /* channel configuration in PWM mode 0 */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    if(config->polarity == PWM_NORMAL_POLARITY) {
        timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    }
    else if(config->polarity == PWM_INVERTED_POLARITY) {
        timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_LOW;
    }
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    for(int i = 0; i < pwmDevice->gd32PwmCfg.ch_num; i ++) {
        timer_channel_output_config(g_tmrMapCmd[pwmDevice->timer], g_tmrchMapCmd[pwmDevice->gd32PwmCfg.channel[i]], &timer_ocintpara);
        timer_channel_output_pulse_value_config(g_tmrMapCmd[pwmDevice->timer], g_tmrchMapCmd[pwmDevice->gd32PwmCfg.channel[i]], 0);
        timer_channel_output_mode_config(g_tmrMapCmd[pwmDevice->timer], g_tmrchMapCmd[pwmDevice->gd32PwmCfg.channel[i]], TIMER_OC_MODE_PWM0);
        timer_channel_output_shadow_config(g_tmrMapCmd[pwmDevice->timer], g_tmrchMapCmd[pwmDevice->gd32PwmCfg.channel[i]], TIMER_OC_SHADOW_DISABLE);
    }
    timer_auto_reload_shadow_enable(g_tmrMapCmd[pwmDevice->timer]);
    timer_enable(g_tmrMapCmd[pwmDevice->timer]);

    for(int i = 0; i < pwmDevice->gd32PwmCfg.ch_num; i ++) {
        timer_channel_output_pulse_value_config(g_tmrMapCmd[pwmDevice->timer], g_tmrchMapCmd[pwmDevice->gd32PwmCfg.channel[i]], config->duty);
    }
}

int32_t Gd32HrpwmSetConfig(struct PwmDev *pwm, struct PwmConfig *config)
{
    GD32HrPwm *pwmDevice = (GD32HrPwm *)pwm->priv;

    if (pwm == NULL || config == NULL) {
        HDF_LOGE("%s: PWM or config is NULL\n", __func__);
        return HDF_FAILURE;
    }

    if(config->status == PWM_ENABLE_STATUS) {
        pwm_timer_config(pwm, config);
    } else {
        /* tmr disable */
        pwm_timer_disable(pwm, config);
    }

    return HDF_SUCCESS;
}

int32_t Gd32PwmOpen(struct PwmDev *pwm)
{
    GD32HrPwm *pwmDevice = (GD32HrPwm *)pwm->priv;

    pwm_gpio_config(pwm);

    if (pwm == NULL) {
        HDF_LOGE("%s: PWM or config is NULL\n", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t Gd32PwmClose(struct PwmDev *pwm)
{
    if (pwm == NULL) {
        HDF_LOGE("%s: PWM or config is NULL\n", __func__);
        return HDF_FAILURE;
    }

    GD32HrPwm *pwmDevice = (GD32HrPwm *)pwm->priv;

    timer_disable(g_tmrMapCmd[pwmDevice->timer]);

    return HDF_SUCCESS;
}

struct PwmMethod g_pwmOps = {
    .setConfig = Gd32HrpwmSetConfig,
    .open = Gd32PwmOpen,
    .close = Gd32PwmClose,
};

static int32_t GD32HrPwmProbe(GD32HrPwm *gd32Hrpwm, struct HdfDeviceObject *obj)
{
    struct DeviceResourceIface *iface = NULL;

    iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (iface == NULL || iface->GetUint32 == NULL || iface->GetUint8 == NULL) {
        HDF_LOGE("%s: face is invalid\n", __func__);
        return HDF_FAILURE;
    }

    Gd32PwmConfig *gd32PwmCfg = &gd32Hrpwm->gd32PwmCfg;
    if (gd32PwmCfg == NULL) {
        HDF_LOGE("%s: gd32PwmCfg is NULL\n", __func__);
        return HDF_FAILURE;
    }
	
    if (iface->GetUint32(obj->property, "num", &(gd32Hrpwm->dev.num), 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read num fail\n", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: gd32Hrpwm->dev.num %d", __func__, gd32Hrpwm->dev.num);

    if (iface->GetUint8(obj->property, "timer", &(gd32Hrpwm->timer), 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read timer fail\n", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: gd32Hrpwm->timer %d", __func__, gd32Hrpwm->timer);

    if (iface->GetUint8(obj->property, "ch_num", &(gd32PwmCfg->ch_num), 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read ch_num fail\n", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: gd32PwmCfg->ch_num %d", __func__, gd32PwmCfg->ch_num);

    gd32PwmCfg->channel = (uint8_t*)OsalMemCalloc((gd32PwmCfg->ch_num) * sizeof(uint8_t));
	 HDF_LOGI("%s: %d", __func__, __LINE__);
    gd32PwmCfg->port = (uint8_t*)OsalMemCalloc((gd32PwmCfg->ch_num) * sizeof(uint8_t));
	 HDF_LOGI("%s: %d", __func__, __LINE__);
    gd32PwmCfg->pin = (uint8_t*)OsalMemCalloc((gd32PwmCfg->ch_num) * sizeof(uint8_t));
	 HDF_LOGI("%s: %d", __func__, __LINE__);
    gd32PwmCfg->func = (uint16_t*)OsalMemCalloc((gd32PwmCfg->ch_num) * sizeof(uint16_t));
	 HDF_LOGI("%s: %d", __func__, __LINE__);

    for (int index = 0; index < gd32PwmCfg->ch_num; index ++) {
        if (iface->GetUint32ArrayElem(obj->property, "channel", index, &(gd32PwmCfg->channel[index]), 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: get channel no(%d) failed", index);
            return HDF_FAILURE;
        }

        if (iface->GetUint32ArrayElem(obj->property, "port", index, &(gd32PwmCfg->port[index]), 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: get port no(%d) failed", index);
            return HDF_FAILURE;
        }

        if (iface->GetUint32ArrayElem(obj->property, "pin", index, &(gd32PwmCfg->pin[index]), 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: get pin no(%d) failed", index);
            return HDF_FAILURE;
        }

        if (iface->GetUint32ArrayElem(obj->property, "func", index, &(gd32PwmCfg->func[index]), 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: get func no(%d) failed", index);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

static int32_t GD32HrPwmBind(struct HdfDeviceObject *obj)
{
    (void *)obj;
    return HDF_SUCCESS;
}

static int32_t GD32HrPwmInit(struct HdfDeviceObject *obj)
{
    int ret;

    if (obj == NULL) {
        HDF_LOGE("%s: obj is null\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    gd32Hrpwm = (GD32HrPwm *)OsalMemCalloc(sizeof(*gd32Hrpwm));
    if (gd32Hrpwm == NULL) {
        HDF_LOGE("%s: OsalMemCalloc gd32Hrpwm error\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GD32HrPwmProbe(gd32Hrpwm, obj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: error probe, ret is %d\n", __func__, ret);
        OsalMemFree(gd32Hrpwm);
    }

    gd32Hrpwm->dev.method = &g_pwmOps;
    gd32Hrpwm->dev.cfg.polarity = 0;
    gd32Hrpwm->dev.cfg.status = PWM_ENABLE_STATUS;
    gd32Hrpwm->dev.cfg.number = 0;
    gd32Hrpwm->dev.busy = false;
    gd32Hrpwm->dev.priv = gd32Hrpwm;
    if (PwmDeviceAdd(obj, &(gd32Hrpwm->dev)) != HDF_SUCCESS) {
        HDF_LOGE("%s: PwmDeviceAdd failed!\n", __func__);
        return HDF_FAILURE;
    }
	


    return ret;
}

static void GD32HrPwmRelease(struct HdfDeviceObject *obj)
{
    GD32HrPwm *gd32Hrpwm = NULL;

    if (obj == NULL) {
        HDF_LOGE("%s: obj is null", __func__);
        return;
    }
    gd32Hrpwm = (struct GD32HrPwm *)obj->service;
    if (gd32Hrpwm == NULL) {
        HDF_LOGE("%s: gd32Hrpwm is null", __func__);
        return;
    }
    PwmDeviceRemove(obj, &(gd32Hrpwm->dev));
    OsalMemFree(gd32Hrpwm);
}

struct HdfDriverEntry g_gd32HdfHrpwm = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_HRPWM",
    .Bind = GD32HrPwmBind,
    .Init = GD32HrPwmInit,
    .Release = GD32HrPwmRelease,
};

HDF_INIT(g_gd32HdfHrpwm);
