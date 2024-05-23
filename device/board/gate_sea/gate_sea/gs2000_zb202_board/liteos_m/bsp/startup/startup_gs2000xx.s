
/**
  ******************************************************************************
  * @file      startup_gs2000xx.s
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

/* Call the clock system initialization function.*/
  bl  SystemInit
/* Call static constructors */
  bl __libc_init_array
/* Call the application's entry point.*/
  bl  main
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
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  HalPendSV
  .word  SysTick_Handler

  /* External Interrupts */
  .word     SYSC_Handler
  .word     WWDG_Handler
  .word     IWDG_Handler
  .word     TMR1_1_Handler
  .word     TMR1_2_Handler
  .word     TMR1_3_Handler
  .word     TMR1_4_Handler
  .word     TMR1_5_Handler
  .word     TMR1_6_Handler
  .word     TMR0_1_Handler
  .word     TMR0_2_Handler
  .word     TMR0_3_Handler
  .word     TMR0_4_Handler
  .word     TMR0_5_Handler
  .word     TMR0_6_Handler
  .word     DMAC_Handler
  .word     DMAC_TC_Handler
  .word     UART0_Handler
  .word     UART1_Handler
  .word     UART2_Handler
  .word     GPIO0_Handler
  .word     GPIO1_Handler
  .word     IIC0_Handler
  .word     UART3_Handler
  .word     IIC1_Handler
  .word     0
  .word     SSP0_Handler
  .word     SSP1_Handler
  .word     ADCC_Handler
  .word     0
  .word     SPI0_Handler
  .word     BPLC_IRQHandler
  .word     NTB_IRQHandler
  .word     CRC_Handler
  .word     MAC_Handler
  .word     AES_Handler
  .word     CRY_Handler
  .word     SHA_Handler
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0
  .word     0//38-70 33个
  .word     EXTI0_Handler
  .word     EXTI1_Handler
  .word     EXTI2_Handler
  .word     EXTI3_Handler
  .word     EXTI4_Handler
  .word     EXTI5_Handler
  .word     EXTI6_Handler
  .word     EXTI7_Handler
  .word     EXTI8_Handler
  .word     EXTI9_Handler
  .word     EXTI10_Handler
  .word     EXTI11_Handler
  .word     EXTI12_Handler
  .word     EXTI13_Handler
  .word     EXTI14_Handler
  .word     EXTI15_Handler
  .word     EXTI16_Handler
  .word     EXTI17_Handler
  .word     EXTI18_Handler
  .word     EXTI19_Handler
  .word     EXTI20_Handler
  .word     EXTI21_Handler
  .word     EXTI22_Handler
  .word     EXTI23_Handler
  .word     EXTI24_Handler


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

   .weak      SVC_Handler
   .thumb_set SVC_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      HalPendSV
   .thumb_set HalPendSV,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler

   .weak      SYSC_Handler
   .thumb_set SYSC_Handler,Default_Handler

   .weak      WWDG_Handler
   .thumb_set WWDG_Handler,Default_Handler

   .weak      IWDG_Handler
   .thumb_set IWDG_Handler,Default_Handler

   .weak      TMR1_1_Handler
   .thumb_set TMR1_1_Handler,Default_Handler

   .weak      TMR1_2_Handler
   .thumb_set TMR1_2_Handler,Default_Handler

   .weak      TMR1_3_Handler
   .thumb_set TMR1_3_Handler,Default_Handler

   .weak      TMR1_4_Handler
   .thumb_set TMR1_4_Handler,Default_Handler

   .weak      TMR1_5_Handler
   .thumb_set TMR1_5_Handler,Default_Handler

   .weak      TMR1_6_Handler
   .thumb_set TMR1_6_Handler,Default_Handler

   .weak      TMR0_1_Handler
   .thumb_set TMR0_1_Handler,Default_Handler

   .weak      TMR0_2_Handler
   .thumb_set TMR0_2_Handler,Default_Handler

   .weak      TMR0_3_Handler
   .thumb_set TMR0_3_Handler,Default_Handler

   .weak      TMR0_4_Handler
   .thumb_set TMR0_4_Handler,Default_Handler

   .weak      TMR0_5_Handler
   .thumb_set TMR0_5_Handler,Default_Handler

   .weak      TMR0_6_Handler
   .thumb_set TMR0_6_Handler,Default_Handler

   .weak      DMAC_Handler
   .thumb_set DMAC_Handler,Default_Handler

   .weak      DMAC_TC_Handler
   .thumb_set DMAC_TC_Handler,Default_Handler

   .weak      UART0_Handler
   .thumb_set UART0_Handler,Default_Handler

   .weak      UART1_Handler
   .thumb_set UART1_Handler,Default_Handler

   .weak      UART2_Handler
   .thumb_set UART2_Handler,Default_Handler

   .weak      GPIO0_Handler
   .thumb_set GPIO0_Handler,Default_Handler

   .weak      GPIO1_Handler
   .thumb_set GPIO1_Handler,Default_Handler

   .weak      IIC0_Handler
   .thumb_set IIC0_Handler,Default_Handler

   .weak      UART3_Handler
   .thumb_set UART3_Handler,Default_Handler

   .weak      IIC1_Handler
   .thumb_set IIC1_Handler,Default_Handler

   .weak      SSP0_Handler
   .thumb_set SSP0_Handler,Default_Handler

   .weak      SSP1_Handler
   .thumb_set SSP1_Handler,Default_Handler

   .weak      ADCC_Handler
   .thumb_set ADCC_Handler,Default_Handler

   .weak      SPI0_Handler
   .thumb_set SPI0_Handler,Default_Handler

   .weak      BPLC_IRQHandler
   .thumb_set BPLC_IRQHandler,Default_Handler

   .weak      NTB_IRQHandler
   .thumb_set NTB_IRQHandler,Default_Handler

   .weak      CRC_Handler
   .thumb_set CRC_Handler,Default_Handler

   .weak      MAC_Handler
   .thumb_set MAC_Handler,Default_Handler

   .weak      AES_Handler
   .thumb_set AES_Handler,Default_Handler

   .weak      CRY_Handler
   .thumb_set CRY_Handler,Default_Handler

   .weak      SHA_Handler
   .thumb_set SHA_Handler,Default_Handler

   .weak      EXTI0_Handler
   .thumb_set EXTI0_Handler,Default_Handler

   .weak      EXTI1_Handler
   .thumb_set EXTI1_Handler,Default_Handler

   .weak      EXTI2_Handler
   .thumb_set EXTI2_Handler,Default_Handler

   .weak      EXTI3_Handler
   .thumb_set EXTI3_Handler,Default_Handler

   .weak      EXTI4_Handler
   .thumb_set EXTI4_Handler,Default_Handler

   .weak      EXTI5_Handler
   .thumb_set EXTI5_Handler,Default_Handler

   .weak      EXTI6_Handler
   .thumb_set EXTI6_Handler,Default_Handler

   .weak      EXTI7_Handler
   .thumb_set EXTI7_Handler,Default_Handler

   .weak      EXTI8_Handler
   .thumb_set EXTI8_Handler,Default_Handler

   .weak      EXTI9_Handler
   .thumb_set EXTI9_Handler,Default_Handler

   .weak      EXTI10_Handler
   .thumb_set EXTI10_Handler,Default_Handler

   .weak      EXTI11_Handler
   .thumb_set EXTI11_Handler,Default_Handler

   .weak      EXTI12_Handler
   .thumb_set EXTI12_Handler,Default_Handler

   .weak      EXTI13_Handler
   .thumb_set EXTI13_Handler,Default_Handler

   .weak      EXTI14_Handler
   .thumb_set EXTI14_Handler,Default_Handler

   .weak      EXTI15_Handler
   .thumb_set EXTI15_Handler,Default_Handler

   .weak      EXTI16_Handler
   .thumb_set EXTI16_Handler,Default_Handler

   .weak      EXTI17_Handler
   .thumb_set EXTI17_Handler,Default_Handler

   .weak      EXTI18_Handler
   .thumb_set EXTI18_Handler,Default_Handler

   .weak      EXTI19_Handler
   .thumb_set EXTI19_Handler,Default_Handler

   .weak      EXTI20_Handler
   .thumb_set EXTI20_Handler,Default_Handler

   .weak      EXTI21_Handler
   .thumb_set EXTI21_Handler,Default_Handler

   .weak      EXTI22_Handler
   .thumb_set EXTI22_Handler,Default_Handler

   .weak      EXTI23_Handler
   .thumb_set EXTI23_Handler,Default_Handler

   .weak      EXTI24_Handler
   .thumb_set EXTI24_Handler,Default_Handler
