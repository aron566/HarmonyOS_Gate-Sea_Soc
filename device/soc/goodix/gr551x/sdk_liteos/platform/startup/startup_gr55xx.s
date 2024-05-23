/**************************************************************************//**
 * @file     startup_gr55xx.s
 * @brief    CMSIS Cortex-M ARMv7-M based Core Device Startup File for
 *           Device GR55xx
 * @version  V1.00
 * @date     10. September 2021
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2021 GOODIX.
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

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

.global  SystemInit
.global  main_init

/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called. 
 * @param  None
 * @retval : None
*/

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  
  ldr   sp, =_estack     /* set stack pointer */
  bl  SystemInit

/* Call the application's entry point.*/
  bl  main_init
  bx  lr    
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an 
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None     
 * @retval None       
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
* 
*******************************************************************************/
  .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler
  
  /* External Interrupts */
  .word  WDT_IRQHandler                   
  .word  BLE_SDK_Handler                  
  .word  BLE_IRQHandler                 
  .word  DMA_IRQHandler                 
  .word  SPI_M_IRQHandler               
  .word  SPI_S_IRQHandler               
  .word  EXT0_IRQHandler                  
  .word  EXT1_IRQHandler                  
  .word  TIMER0_IRQHandler                
  .word  TIMER1_IRQHandler 
  .word  DUAL_TIMER_IRQHandler
  .word  QSPI0_IRQHandler
  .word  UART0_IRQHandler      
  .word  UART1_IRQHandler
  .word  I2C0_IRQHandler
  .word  I2C1_IRQHandler
  .word  AES_IRQHandler
  .word  HMAC_IRQHandler
  .word  EXT2_IRQHandler
  .word  RNG_IRQHandler
  .word  PMU_IRQHandler
  .word  PKC_IRQHandler
  .word  XQSPI_IRQHandler
  .word  QSPI1_IRQHandler
  .word  PWR_CMD_IRQHandler
  .word  BLESLP_IRQHandler
  .word  SLPTIMER_IRQHandler
  .word  COMP_IRQHandler
  .word  AON_WDT_IRQHandler
  .word  I2S_M_IRQHandler
  .word  I2S_S_IRQHandler
  .word  ISO7816_IRQHandler
  .word  PRESENT_IRQHandler
  .word  CALENDAR_IRQHandler

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler. 
* As they are weak aliases, any function with the same name will override 
* this definition.
* 
*******************************************************************************/
   .weak      NMI_Handler
   .thumb_set NMI_Handler,Default_Handler

   .weak      HardFault_Handler
   .thumb_set HardFault_Handler,Default_Handler

   .weak      MemManage_Handler
   .thumb_set MemManage_Handler,Default_Handler

   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      PendSV_Handler
   .thumb_set PendSV_Handler,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler              

   .weak      WDT_IRQHandler                   
   .thumb_set WDT_IRQHandler,Default_Handler      

   .weak      BLE_SDK_Handler      
   .thumb_set BLE_SDK_Handler,Default_Handler

   .weak      BLE_IRQHandler            
   .thumb_set BLE_IRQHandler,Default_Handler

   .weak      DMA_IRQHandler                  
   .thumb_set DMA_IRQHandler,Default_Handler

   .weak      SPI_M_IRQHandler         
   .thumb_set SPI_M_IRQHandler,Default_Handler

   .weak      SPI_S_IRQHandler      
   .thumb_set SPI_S_IRQHandler,Default_Handler

   .weak      EXT0_IRQHandler         
   .thumb_set EXT0_IRQHandler,Default_Handler

   .weak      EXT1_IRQHandler         
   .thumb_set EXT1_IRQHandler,Default_Handler

   .weak      TIMER0_IRQHandler         
   .thumb_set TIMER0_IRQHandler,Default_Handler 

   .weak      TIMER1_IRQHandler         
   .thumb_set TIMER1_IRQHandler,Default_Handler

   .weak      DUAL_TIMER_IRQHandler         
   .thumb_set DUAL_TIMER_IRQHandler,Default_Handler

   .weak      QSPI0_IRQHandler               
   .thumb_set QSPI0_IRQHandler,Default_Handler

   .weak      UART0_IRQHandler               
   .thumb_set UART0_IRQHandler,Default_Handler

   .weak      UART1_IRQHandler               
   .thumb_set UART1_IRQHandler,Default_Handler

   .weak      I2C0_IRQHandler               
   .thumb_set I2C0_IRQHandler,Default_Handler 

   .weak      I2C1_IRQHandler              
   .thumb_set I2C1_IRQHandler,Default_Handler

   .weak      AES_IRQHandler               
   .thumb_set AES_IRQHandler,Default_Handler

   .weak      HMAC_IRQHandler               
   .thumb_set HMAC_IRQHandler,Default_Handler

   .weak      EXT2_IRQHandler      
   .thumb_set EXT2_IRQHandler,Default_Handler

   .weak      RNG_IRQHandler   
   .thumb_set RNG_IRQHandler,Default_Handler

   .weak      PMU_IRQHandler                  
   .thumb_set PMU_IRQHandler,Default_Handler

   .weak      PKC_IRQHandler                  
   .thumb_set PKC_IRQHandler,Default_Handler

   .weak      XQSPI_IRQHandler                  
   .thumb_set XQSPI_IRQHandler,Default_Handler

   .weak      QSPI1_IRQHandler   
   .thumb_set QSPI1_IRQHandler,Default_Handler

   .weak      PWR_CMD_IRQHandler            
   .thumb_set PWR_CMD_IRQHandler,Default_Handler

   .weak      BLESLP_IRQHandler            
   .thumb_set BLESLP_IRQHandler,Default_Handler

   .weak      SLPTIMER_IRQHandler      
   .thumb_set SLPTIMER_IRQHandler,Default_Handler

   .weak      COMP_IRQHandler   
   .thumb_set COMP_IRQHandler,Default_Handler

   .weak      AON_WDT_IRQHandler            
   .thumb_set AON_WDT_IRQHandler,Default_Handler

   .weak      I2S_M_IRQHandler            
   .thumb_set I2S_M_IRQHandler,Default_Handler
   
   .weak      I2S_S_IRQHandler            
   .thumb_set I2S_S_IRQHandler,Default_Handler

   .weak      ISO7816_IRQHandler   
   .thumb_set ISO7816_IRQHandler,Default_Handler

   .weak      PRESENT_IRQHandler   
   .thumb_set PRESENT_IRQHandler,Default_Handler

   .weak      CALENDAR_IRQHandler   
   .thumb_set CALENDAR_IRQHandler,Default_Handler

   .global SVC_handler_proc
   .section  .ramfunc
   .type SVC_Handler, %function
   .global SVC_Handler
SVC_Handler:
    TST R14,$4
    IT NE
    MRSNE R12,PSP
    IT EQ
    MOVEQ R12,SP
    PUSH  {R0-R3,LR}
    MOV  R0, R12
    BL  SVC_handler_proc
    MOV  R12, R0
    POP {R0-R3,LR}
    CMP R12,$0
    IT NE
    BLXNE  R12
    BX  LR

/*****END OF FILE****/
  

