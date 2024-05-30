/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                         STM3240G-EVAL
*                                        Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef BSP_PRESENT
#define BSP_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef BSP_MODULE
  #define BSP_EXT
#else
  #define BSP_EXT extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "GSMCU_M3.h"
#include "bps_flash.h"
#include "apdma.h"
#include "bps_led.h"
#include "bps_zerocross.h"
#include "bps_port.h"
#include "bsp_wdg.h"
#include "bps_uart.h"
#include "bps_timer.h"
#include "bps_debug.h"
#include "gsmcu_iwdg.h"
//=======================

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/

void SYSC_Handler(void);
void WWDG_Handler(void);
void IWDG_Handler(void);

void BSP_Init(void);

#define BPS_PHASE_A 0
#define BPS_PHASE_B 1
#define BPS_PHASE_C 2

#define BPS_PHASE_A_NEG 3
#define BPS_PHASE_B_NEG 4
#define BPS_PHASE_C_NEG 5

#define METER_UART_DMA_CHL DMA_Channel0
#define DEBUG_UART_DMA_CHL DMA_Channel1
extern uint8_t BootPowerState;
/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/
#endif /* End of module include.                               */
