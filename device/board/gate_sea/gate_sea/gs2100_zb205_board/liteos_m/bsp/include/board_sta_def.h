/************************************************************************************************
GateSea HPLC STA Board Defines
************************************************************************************************/
#ifndef BOARD_DEF
#define BOARD_DEF

#define GS_STA 1u
// #define ALL_USE_TESTMODE_PARAM
#define FACTORY_CODE      "GS"
#define CHIP_CODE         "Z5"
#define FACTORY_CODE_FLAG 0x80546867
#define CPU_NAME          "ZB205"

#define DEV_CCO 0u
#define DEV_STA 1u

// 模块软件版本信息
#define MODULE_SOFT_VER           (0x1001)
#define MODULE_SOFT_VER_DATE_YEAR (23)
#define MODULE_SOFT_VER_DATE_MON  (12)
#define MODULE_SOFT_VER_DATE_DAY  (14)

// 资产信息南网科技备案信息
// 模块硬件版本信息
#define MODULE_HARD_VER           (0x1001)
#define MODULE_HARD_VER_DATE_YEAR (23)
#define MODULE_HARD_VER_DATE_MON  (8)
#define MODULE_HARD_VER_DATE_DAY  (28)
// 芯片硬件版本信息
#define CHIP_HARD_VER           (0x1001)
#define CHIP_HARD_VER_DATE_YEAR (23)
#define CHIP_HARD_VER_DATE_MON  (8)
#define CHIP_HARD_VER_DATE_DAY  (28)
// 芯片软件版本信息
#define CHIP_SOFT_VER           (0x1001)
#define CHIP_SOFT_VER_DATE_YEAR (23)
#define CHIP_SOFT_VER_DATE_MON  (8)
#define CHIP_SOFT_VER_DATE_DAY  (28)
// 应用程序版本号
#define APP_VER (0x0011)

#if defined(ZB204_CHIP)
  #include "ZB204.h"
#elif defined(ZB205_CHIP)
  #include "ZB205.h"
#endif

#define GREEN_RX_LED_PORT 1
#define GREEN_RX_LED_PIN  6

#define RED_TX_LED_PORT 6
#define RED_TX_LED_PIN  0

#define GREEN_RX_LED_PORT_3PHASE 1
#define GREEN_RX_LED_PIN_3PHASE  6

#define RED_TX_LED_PORT_3PHASE 6
#define RED_TX_LED_PIN_3PHASE  0

#define STAOUT_PORT 1
#define STAOUT_PIN  3

#define STAOUT_PORT_3PHASE 1
#define STAOUT_PIN_3PHASE  3

#define EVENT_PORT 1
#define EVENT_PIN  2

#define EVENT_PORT_3PHASE 1
#define EVENT_PIN_3PHASE  2

#define SET_PORT 1
#define SET_PIN  5

#define SET_PORT_3PHASE 1
#define SET_PIN_3PHASE  5

#define RESET_PORT 1
#define RESET_PIN  4

#define RESET_PORT_3PHASE 1
#define RESET_PIN_3PHASE  4

#define MODE_PIN1_PORT 0
#define MODE_PIN1      0
#define MODE_PIN1_MODE 1   // 非IO脚，模式选择 =1：外部下拉； =0：外部上拉

#define LD_PORT 3
#define LD_PIN  7

#define LD_PORT_3PHASE 3
#define LD_PIN_3PHASE  7

#define HRF_DIR_PORT 6
#define HRF_DIR_PIN  1
#define HRF_DIR_MODE 1   // 非IO脚，模式选择 =0：0发送1接收； =1：1发送0接收

#define PULLOUT_PORT 1
#define PULLOUT_PIN  7

#define PULLOUT_PORT_3PHASE 1
#define PULLOUT_PIN_3PHASE  7

#define TOPO_PORT 3
#define TOPO_PIN  7

#define TOPO_PORT_3PHASE 3
#define TOPO_PIN_3PHASE  7

#define MULTI_PORT 0
#define MULTI_PIN  3

#define MULTI_PORT_3PHASE 0
#define MULTI_PIN_3PHASE  3

// #define METER_UART_OPEN_DRAIN
// ZCPORTs must be PORT1
#define ZC_PIN_PHASEA_POS 0
#define ZC_PIN_PHASEB_POS 4
#define ZC_PIN_PHASEC_POS 5

#define ZC_CH_PHASEA_POS 1
#define ZC_CH_PHASEB_POS 5
#define ZC_CH_PHASEC_POS 6
#define ZC_CH_PHASEA_NEG 2
#define ZC_CH_PHASEB_NEG 3
#define ZC_CH_PHASEC_NEG 4

// #define HRF_TX_LED_PORT                         2
// #define HRF_TX_LED_PIN                          2
// #define HRF_RX_LED_PORT                         2
// #define HRF_RX_LED_PIN                          3

#define TOPO_PWM_TMR 0

#define DCDC_VOUT_VAL 3   // 0:1.5V; 1:1.6V; 2:1.3V ; 3:1.4V

#define METER_UART Driver_USART3
#define DEBUG_UART Driver_USART0

// 1下降沿 2上升沿
#define ZERO_EDGE_AREA 2
// 0下降沿 1上升沿
#define ZERO_EDGE_SHANXI_ASK 1
#endif
