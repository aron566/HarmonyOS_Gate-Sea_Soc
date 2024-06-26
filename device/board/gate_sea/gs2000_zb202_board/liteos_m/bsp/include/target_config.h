/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "los_compiler.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* =============================================================================
                                        System clock module configuration
============================================================================= */
#define OS_SYS_CLOCK                                        SystemCoreClock
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (1000UL)
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       0
#define LOSCFG_BASE_CORE_TICK_WTIMER                        0
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                  (0xFFFFFFUL)

/* =============================================================================
                                        Hardware interrupt module configuration
============================================================================= */
#define LOSCFG_PLATFORM_HWI                                 1
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT                 0
#define LOSCFG_PLATFORM_HWI_LIMIT                           96

/* =============================================================================
                                       Task module configuration
============================================================================= */
#define LOSCFG_BASE_CORE_TSK_LIMIT                          20
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                (0x400U)
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             (0x400U)
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (ALIGN(0x400, 4))
#define LOSCFG_BASE_CORE_TIMESLICE                          1
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  10
#define LOSCFG_BASE_CORE_TSK_MONITOR                        1
#define LOSCFG_BASE_CORE_EXC_TSK_SWITCH                     1

/* =============================================================================
                                       Semaphore module configuration
============================================================================= */
#define LOSCFG_BASE_IPC_SEM                                 1
#define LOSCFG_BASE_IPC_SEM_LIMIT                           20
/* =============================================================================
                                       Mutex module configuration
============================================================================= */
#define LOSCFG_BASE_IPC_MUX                                 1
// increase to 48, os fundermental + Devmanager + hievent + samgr cost 36~ mux
#define LOSCFG_BASE_IPC_MUX_LIMIT                           20
/* =============================================================================
                                       Queue module configuration
============================================================================= */
#define LOSCFG_BASE_IPC_QUEUE                               1
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         20
/* =============================================================================
                                       Software timer module configuration
============================================================================= */
#define LOSCFG_BASE_CORE_SWTMR                              1
#define LOSCFG_BASE_CORE_SWTMR_ALIGN                        1
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        20
/* =============================================================================
                                       Exception module configuration
============================================================================= */
#define LOSCFG_PLATFORM_EXC                                 0
/* =============================================================================
                                       Memory module configuration
============================================================================= */
extern unsigned int __los_heap_addr_start__;
extern unsigned int __los_heap_addr_end__;
#define LOSCFG_SYS_EXTERNAL_HEAP                            1
#define LOSCFG_SYS_HEAP_ADDR                                ((void *)&__los_heap_addr_start__)
#define LOSCFG_SYS_HEAP_SIZE                                (((unsigned int)&__los_heap_addr_end__) - ((unsigned int)&__los_heap_addr_start__))
#define LOSCFG_MEM_MUL_POOL                                 1
#define OS_SYS_MEM_NUM                                      32
#define LOSCFG_MEM_FREE_BY_TASKID                           1
#define LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK                0
#define LOSCFG_MEM_LEAKCHECK                                0
/* =============================================================================
                                       printf module configuration
============================================================================= */
#define LOSCFG_KERNEL_PRINTF                                1/**< 0不启用内核打印，1启用内核打印 2只启用shell控制台打印 */

//#define LOSCFG_FS_VFS                             1
// #define LOSCFG_SUPPORT_LITTLEFS                             0 //in config.h at out folder

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _TARGET_CONFIG_H */
