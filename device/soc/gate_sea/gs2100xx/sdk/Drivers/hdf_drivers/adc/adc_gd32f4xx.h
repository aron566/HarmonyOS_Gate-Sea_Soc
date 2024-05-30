#ifndef _ADC_BASE_HAL_H
#define _ADC_BASE_HAL_H

#include "stdbool.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "adc_core.h"
#include "hal_gpio.h"

#define ADC_CHANNEL_MAX    18    //ADC通道最大数
#define ADC_TYPE_MAX      3      //0-2总共3路ADC
#define DMA_BLOCK_SIZE    16     //0-15 总共16个通道数据

typedef struct {
    uint32_t adchannel;
    uint32_t pin;
    uint32_t group;
} AdcInitRes;

struct gd32f4xxAdcDevice {
    struct AdcDevice device;
    uint32_t deviceNum;
    uint16_t AdcType;
    uint16_t RcuTimer;
    uint16_t RcuDma;
    uint16_t AdcAdcck;
    uint32_t AdcAlign;
    uint16_t AdcSequence;
    uint32_t AdcExtTrigger;
    uint32_t AdcExtTrMode;
    uint16_t DmaType;
    uint16_t DmaChannel;
    uint16_t DmaIncreasMode;
    uint16_t MemIncreasMode;
    uint16_t DmaTransWidth;
    uint16_t DmaTransMode;
    uint16_t DmaChPriority;
    uint16_t DmaSubperi;
    uint16_t AdcSampleTime;
    uint16_t ChannelCount;
    AdcInitRes* AdcConfLst;
};

static const uint16_t g_rcu_adc_Maps[3] = {
    RCU_ADC0,
    RCU_ADC1,
    RCU_ADC2,
};

static const uint16_t g_rcu_timer_Maps[13] = {
    RCU_TIMER1,
    RCU_TIMER2,
    RCU_TIMER3,
    RCU_TIMER4,
    RCU_TIMER5,
    RCU_TIMER6,
    RCU_TIMER7,
    RCU_TIMER8,
    RCU_TIMER9,
    RCU_TIMER10,
    RCU_TIMER11,
    RCU_TIMER12,
    RCU_TIMER13,
};

static const uint16_t g_rcu_dma_Maps[2] = {
    RCU_DMA0,
    RCU_DMA1,
};

static const uint32_t g_dma_type_Maps[2] = {
    DMA0,
    DMA1,
};

static const uint32_t g_adc_type_Maps[3] = {
    ADC0,
    ADC1,
    ADC2,
};

static const uint16_t g_adc_channel_Maps[19] = {
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
    ADC_CHANNEL_10,
    ADC_CHANNEL_11,
    ADC_CHANNEL_12,
    ADC_CHANNEL_13,
    ADC_CHANNEL_14,
    ADC_CHANNEL_15,
    ADC_CHANNEL_16,
    ADC_CHANNEL_17,
    ADC_CHANNEL_18,
};

static const uint32_t g_adc_dataalign_Maps[2] = {
    ADC_DATAALIGN_RIGHT,
    ADC_DATAALIGN_LEFT,
};

static const uint16_t g_adc_sequence_Maps[4] = {
    ADC_ROUTINE_CHANNEL,
    ADC_INSERTED_CHANNEL,
    ADC_ROUTINE_INSERTED_CHANNEL,
    ADC_CHANNEL_DISCON_DISABLE,
};

static const uint32_t g_adc_ext_trigger_Maps[7] = {
    ADC_EXTTRIG_ROUTINE_T0_CH0,
    ADC_EXTTRIG_ROUTINE_T0_CH1,
    ADC_EXTTRIG_ROUTINE_T0_CH2,
    ADC_EXTTRIG_ROUTINE_T1_CH1,
    ADC_EXTTRIG_ROUTINE_T1_CH2,
    ADC_EXTTRIG_ROUTINE_T1_CH3,
    ADC_EXTTRIG_ROUTINE_T1_TRGO,
};

static const uint32_t g_adc_ext_trigger_mode_Maps[4] = {
    EXTERNAL_TRIGGER_DISABLE,
    EXTERNAL_TRIGGER_RISING,
    EXTERNAL_TRIGGER_FALLING,
    EXTERNAL_TRIGGER_RISING_FALLING,
};

#endif

