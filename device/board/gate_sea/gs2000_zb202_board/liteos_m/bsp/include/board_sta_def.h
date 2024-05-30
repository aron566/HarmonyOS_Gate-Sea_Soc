/************************************************************************************************
GateSea HPLC STA Board Defines
************************************************************************************************/
#ifndef BOARD_DEF
#define BOARD_DEF

#define GS_STA 1u
// #define ALL_USE_TESTMODE_PARAM
#define FACTORY_CODE      "GS"
#define CHIP_CODE         "Z2"
#define FACTORY_CODE_FLAG 0x535A4753
#define CPU_NAME          "ZB202"
#define ZB202             1U
#define DM750             0U

#define DEV_CCO 0u
#define DEV_STA 1u

#define MODULE_SOFT_VER           ((SLG_COMMIT_HASH >> 12) & 0xffff)
#define MODULE_SOFT_VER_DATE_YEAR SLG_COMMIT_YEAR
#define MODULE_SOFT_VER_DATE_MON  SLG_COMMIT_MON
#define MODULE_SOFT_VER_DATE_DAY  SLG_COMMIT_DATE

#include <iogsmcu.h>

#define GREEN_RX_LED_PORT_USE_PORT0
#define GREEN_RX_LED_PIN GPIO_Pin_19

#define RED_TX_LED_PORT_USE_PORT0
#define RED_TX_LED_PIN GPIO_Pin_18

#define GREEN_RX_LED_PORT_3PHASE_USE_PORT0
#define GREEN_RX_LED_PIN_3PHASE GPIO_Pin_19

#define RED_TX_LED_PORT_3PHASE_USE_PORT0
#define RED_TX_LED_PIN_3PHASE GPIO_Pin_18

#define STAOUT_PORT_USE_PORT0
#define STAOUT_PIN GPIO_Pin_25

#define STAOUT_PORT_3PHASE_USE_PORT0
#define STAOUT_PIN_3PHASE GPIO_Pin_25

#define EVENT_PORT_USE_PORT0
#define EVENT_PIN GPIO_Pin_24

#define EVENT_PORT_3PHASE_USE_PORT0
#define EVENT_PIN_3PHASE GPIO_Pin_24

#define RESET_PORT_USE_PORT0
#define RESET_PIN GPIO_Pin_22

#define RESET_PORT_3PHASE_USE_PORT0
#define RESET_PIN_3PHASE GPIO_Pin_22

#define MODE_PIN1_PORT_USE_PORT0
#define MODE_PIN2_PORT_USE_PORT0
#define MODE_PIN1 GPIO_Pin_20
#define MODE_PIN2 GPIO_Pin_21

#define MODE_PIN1_PORT 1   // 0外部上拉，1外部下拉

#define LD_PORT_USE_PORT0
#define LD_PIN GPIO_Pin_31

#define LD_PORT_3PHASE_USE_PORT0
#define LD_PIN_3PHASE GPIO_Pin_31

#define PULLOUT_PORT_USE_PORT0
#define PULLOUT_PIN GPIO_Pin_30

#define PULLOUT_PORT_3PHASE_USE_PORT0
#define PULLOUT_PIN_3PHASE GPIO_Pin_30

#define TOPO_PORT_USE_PORT0
#define TOPO_PIN GPIO_Pin_30

#define TOPO_PORT_3PHASE_USE_PORT0
#define TOPO_PIN_3PHASE GPIO_Pin_30

#define METER_UART_USE_UART0 Driver_USART0
#define METER_UART_DMA_CHL   DMA_Channel0
#define METER_UART_OPEN_DRAIN

// ZCPORTs must be PORT1
#define ZC_PIN_PHASEA_POS GPIO_Pin_15
#define ZC_PIN_PHASEB_POS GPIO_Pin_14
#define ZC_PIN_PHASEC_POS GPIO_Pin_13

#define ZC_PHASEA_POS_IRQn EXTI15_IRQn
#define ZC_PHASEB_POS_IRQn EXTI14_IRQn
#define ZC_PHASEC_POS_IRQn EXTI13_IRQn

#define ZC_CH_PHASEA_POS 2
#define ZC_CH_PHASEB_POS 1
#define ZC_CH_PHASEC_POS 0

#define POWERDET_ADC_CHL 5

#define DEBUG_PORT_USE_UART1

// 1下降沿 2上升沿
#define ZERO_EDGE_AREA 1
// 0下降沿 1上升沿
#define ZERO_EDGE_SHANXI_ASK 0

#endif
