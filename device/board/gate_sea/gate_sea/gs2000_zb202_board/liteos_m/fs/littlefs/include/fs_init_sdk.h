/**
 *  @file fs_init_sdk.h
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.27 16:06 初始版本.
 *  @date 2024-05-27
 *
 *
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-27 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
#ifndef FS_INIT_SDK_H
#define FS_INIT_SDK_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
/** Private includes ---------------------------------------------------------*/

/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
#define READ_SIZE 1024
#define PROG_SIZE 1024
#define CACHE_SIZE 2048
#define LOOKAHEAD_SIZE 2048
#define BLOCK_CYCLES 500
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

extern int32_t FsInit(void);

#ifdef __cplusplus /**< end extern c */
}
#endif
#endif
/******************************** End of file *********************************/
