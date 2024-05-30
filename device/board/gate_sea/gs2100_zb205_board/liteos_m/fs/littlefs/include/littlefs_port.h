
/**
 *  @file littlefs_port.h
 *  @brief None.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.27 13:34 初始版本.
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
#ifndef LITTLEFS_PORT_H
#define LITTLEFS_PORT_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
/** Private includes ---------------------------------------------------------*/
#include "lfs.h"

/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
int LittlefsRead(int partition, uint32_t *offset, void *buf, uint32_t size);
int LittlefsProg(int partition, uint32_t *offset, const void *buf, uint32_t size);
int LittlefsErase(int partition, uint32_t offset, uint32_t size);

/**
 * @brief 获取作为littlsfs flash起始地址
 *
 * @return uint32_t 起始地址
 */
uint32_t Littlefs_Port_Get_Start_Addr(void);

/**
 * @brief 获取littlefs 空间大小
 *
 * @return uint32_t
 */
uint32_t Littlefs_Port_Get_Littlefs_Size(void);

/**
 * @brief 获取littlefs 块数量
 *
 * @return uint32_t
 */
uint32_t Littlefs_Port_Get_Littlefs_Blocks(void);

#ifdef __cplusplus /**< end extern c */
}
#endif
#endif
/******************************** End of file *********************************/
