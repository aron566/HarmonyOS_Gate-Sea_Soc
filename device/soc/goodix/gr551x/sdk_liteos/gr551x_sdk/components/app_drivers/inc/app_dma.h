/**
 ****************************************************************************************
 *
 * @file    app_dma.h
 * @author  BLE Driver Team
 * @brief   Header file containing functions prototypes of DMA app library.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************
 */

/** @addtogroup PERIPHERAL Peripheral Driver
  * @{
  */

/** @addtogroup APP_DRIVER APP DRIVER
 *  @{
 */

/** @defgroup APP_DMA DMA
  * @brief DMA APP module driver.
  * @{
  */


#ifndef _APP_DMA_H_
#define _APP_DMA_H_

#include "gr55xx_hal.h"
#include "app_drv_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_DMA_MODULE_ENABLED

/** @addtogroup APP_DMA_ENUMERATIONS Enumerations
  * @{
  */
/**
  * @brief DMA event Enumerations definition
  */
typedef enum {
    APP_DMA_EVT_ERROR,                     /**< The event of error interrupt. */
    APP_DMA_EVT_TFR,                       /**< The event of transfer complete interrupt. */
} app_dma_evt_type_t;
/** @} */

/** @addtogroup APP_DMA_STRUCTURES Structures
  * @{
  */
/**
  * @brief DMA parameters structure definition
  */
typedef struct {
    dma_channel_t       channel_number;    /**< Specifies the channel of DMA. */
    dma_init_t          init;              /**< DMA communication parameters. */
} app_dma_params_t;
/** @} */

/** @addtogroup APP_DMA_TYPEDEFS Typedefs
  * @{
  */
/**
  * @brief DMA event callback definition
  */
typedef void (*app_dma_evt_handler_t)(app_dma_evt_type_t type);

/** @} */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup APP_DMA_DRIVER_FUNCTIONS Functions
  * @{
  */
/**
 ****************************************************************************************
 * @brief  Initialize the APP DMA DRIVER according to the specified parameters
 *         in the app_dma_params_t and app_dma_evt_handler_t.
 *
 * @param[in]  p_params: Pointer to app_dma_params_t parameter which contains the
 *                       configuration information for the specified DMA module.
 * @param[in]  evt_handler: DMA user callback function.
 *
 * @return DMA ID
 ****************************************************************************************
 */
int16_t app_dma_init(app_dma_params_t *p_params, app_dma_evt_handler_t evt_handler);

/**
 ****************************************************************************************
 * @brief  De-initialize the APP ADC DRIVER peripheral.
 *
 * @param[in]  ins_id: Deinitialize DMA channel for a specific ID.
 *
 * @return Result of De-initialization.
 ****************************************************************************************
 */
uint16_t app_dma_deinit(int16_t ins_id);

/**
 ****************************************************************************************
 * @brief  Start the DMA Transfer.
 *
 * @param[in]  id: DMA channel id.
 * @param[in]  src_address: The source memory Buffer address
 * @param[in]  dst_address: The destination memory Buffer address
 * @param[in]  data_length: The length of data to be transferred from source to destination, ranging between 0 and 4095.
 ****************************************************************************************
 */
uint16_t app_dma_start(int16_t id, uint32_t src_address, uint32_t dst_address, uint32_t data_length);

/**
 ****************************************************************************************
 * @brief  Return the DMA handle.
 *
 * @param[in]  id: DMA Channel ID.
 *
 * @return Pointer to the specified ID's DMA handle.
 ****************************************************************************************
 */
dma_handle_t *app_dma_get_handle(int16_t id);

#ifdef APP_DRIVER_WAKEUP_CALL_FUN
/**
 ****************************************************************************************
 * @brief  resume dma after wake up for other modules.
 *
 * @param[in]  id: DMA Channel ID.
 *
 * @return void.
 ****************************************************************************************
 */
void dma_wake_up(int16_t id);
#endif


/** @} */

#endif

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
/** @} */
