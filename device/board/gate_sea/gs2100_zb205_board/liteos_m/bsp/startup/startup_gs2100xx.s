
/**
  ******************************************************************************
  * @file      startup_gs2100xx.s
  * @author    Gate-Sea Application Team
  * @brief     gs2100xx Devices vector table for GCC based toolchains.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M3 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 Gate-Sea.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

.extern HalExcNMI;
.extern HalExcHardFault;
.extern HalExcMemFault;
.extern HalExcBusFault;
.extern HalExcUsageFault;
.extern HalExcSvcCall;
.extern HalPendSV;
.extern SysTick_Handler;


.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */
.equ  BootRAM,        0xF1E0F85F
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

/* Call the clock system initialization function.*/
  bl  SystemInit

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call static constructors */
  bl __libc_init_array
/* Call the application's entry point.*/
  bl  main

LoopForever:
  b LoopForever

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
  .word  HalExcNMI
  .word  HalExcHardFault
  .word  HalExcMemFault
  .word  HalExcBusFault
  .word  HalExcUsageFault
  .word  0
  .word  0
  .word  0
  .word  0
  .word  HalExcSvcCall
  .word  DebugMon_Handler
  .word  0
  .word  HalPendSV
  .word  SysTick_Handler

  /* External Interrupts */
  .word apDMA_RawISR //0 DMA
  .word ntb_interrupt_handler //1
  .word bplc_interrupt_handler //2
  .word 0  //3
  .word 0  //4
  .word 0  //5
  .word USART0_IRQHandler //6
  .word USART1_IRQHandler //7
  .word USART2_IRQHandler //8
  .word USART3_IRQHandler //9
  .word 0  //10
  .word 0  //11
  .word 0  //12
  .word 0  //13
  .word 0  //14
  .word HRF_interrupt_handler //15
  .word WWDG_Handler  //16
  .word 0  //17
  .word 0  //18
  .word 0  //19
  .word BPS_GPIO0_IRQ //20
  .word BPS_GPIO1_IRQ //21
  .word BPS_GPIO2_IRQ //22
  .word BPS_GPIO3_IRQ //23
  .word BPS_GPIO4_IRQ //24
  .word BPS_GPIO5_IRQ //25
  .word BPS_GPIO6_IRQ //26
  .word BPS_GPIO7_IRQ //27
  .word BPS_GPIO8_IRQ //28
  .word BPS_GPIO9_IRQ //29
  .word Timer0IsrFunction //30
  .word Timer1IsrFunction //31
  .word Timer2IsrFunction //32
  .word Timer3IsrFunction //33
  .word Timer4IsrFunction //34
  .word Timer5IsrFunction //35
  .word Timer6IsrFunction //36
  .word Timer7IsrFunction //37
  .word Timer8IsrFunction //38
  .word Timer9IsrFunction //39
  .word Timer10IsrFunction //40
  .word Timer11IsrFunction //41
  .word DetectADC12VIrq//42
  .word Default_Handler//0  //43
  .word Default_Handler//0  //44
  .word HRF_dfe_interrupt_handler //45


/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/
   .weak      HalExcNMI
   .thumb_set HalExcNMI,Default_Handler

   .weak      HalExcHardFault
   .thumb_set HalExcHardFault,Default_Handler

   .weak      HalExcMemFault
   .thumb_set HalExcMemFault,Default_Handler

   .weak      HalExcBusFault
   .thumb_set HalExcBusFault,Default_Handler

   .weak      HalExcUsageFault
   .thumb_set HalExcUsageFault,Default_Handler

   .weak      HalExcSvcCall
   .thumb_set HalExcSvcCall,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      HalPendSV
   .thumb_set HalPendSV,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler

  .weak      apDMA_RawISR
  .thumb_set apDMA_RawISR,Default_Handler

  .weak      WWDG_Handler
  .thumb_set WWDG_Handler,Default_Handler

  .weak      SPI0_IRQHandler
  .thumb_set SPI0_IRQHandler,Default_Handler

  .weak      Timer0IsrFunction
  .thumb_set Timer0IsrFunction,Default_Handler

  .weak      Timer1IsrFunction
  .thumb_set Timer1IsrFunction,Default_Handler

  .weak      Timer2IsrFunction
  .thumb_set Timer2IsrFunction,Default_Handler

  .weak      Timer3IsrFunction
  .thumb_set Timer3IsrFunction,Default_Handler

  .weak      Timer4IsrFunction
  .thumb_set Timer4IsrFunction,Default_Handler

  .weak      Timer5IsrFunction
  .thumb_set Timer5IsrFunction,Default_Handler

  .weak      Timer6IsrFunction
  .thumb_set Timer6IsrFunction,Default_Handler

  .weak      Timer7IsrFunction
  .thumb_set Timer7IsrFunction,Default_Handler

  .weak      Timer8IsrFunction
  .thumb_set Timer8IsrFunction,Default_Handler

  .weak      Timer9IsrFunction
  .thumb_set Timer9IsrFunction,Default_Handler

  .weak      Timer10IsrFunction
  .thumb_set Timer10IsrFunction,Default_Handler

  .weak      Timer11IsrFunction
  .thumb_set Timer11IsrFunction,Default_Handler

  .weak      HRF_interrupt_handler
  .thumb_set HRF_interrupt_handler,Default_Handler

  .weak      HRF_dfe_interrupt_handler
  .thumb_set HRF_dfe_interrupt_handler,Default_Handler

  .weak      ntb_interrupt_handler
  .thumb_set ntb_interrupt_handler,Default_Handler

  .weak      bplc_interrupt_handler
  .thumb_set bplc_interrupt_handler,Default_Handler

  .weak      DetectADC12VIrq
  .thumb_set DetectADC12VIrq,Default_Handler

  .weak      USART0_IRQHandler
  .thumb_set USART0_IRQHandler,Default_Handler

  .weak      USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler

  .weak      USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler

  .weak      USART3_IRQHandler
  .thumb_set USART3_IRQHandler,Default_Handler

  .weak      BPS_GPIO0_IRQ
  .thumb_set BPS_GPIO0_IRQ,Default_Handler

  .weak      BPS_GPIO1_IRQ
  .thumb_set BPS_GPIO1_IRQ,Default_Handler

  .weak      BPS_GPIO2_IRQ
  .thumb_set BPS_GPIO2_IRQ,Default_Handler

  .weak      BPS_GPIO3_IRQ
  .thumb_set BPS_GPIO3_IRQ,Default_Handler

  .weak      BPS_GPIO4_IRQ
  .thumb_set BPS_GPIO4_IRQ,Default_Handler

  .weak      BPS_GPIO5_IRQ
  .thumb_set BPS_GPIO5_IRQ,Default_Handler

  .weak      BPS_GPIO6_IRQ
  .thumb_set BPS_GPIO6_IRQ,Default_Handler

  .weak      BPS_GPIO7_IRQ
  .thumb_set BPS_GPIO7_IRQ,Default_Handler

  .weak      BPS_GPIO8_IRQ
  .thumb_set BPS_GPIO8_IRQ,Default_Handler

  .weak      BPS_GPIO9_IRQ
  .thumb_set BPS_GPIO9_IRQ,Default_Handler
