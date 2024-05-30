/**
 *  @file uart.c
 *  @brief 串口接口.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.22 16:14 初始版本.
 *  @date 2024-05-22
 *
 *  @details None.
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2024-05-22 <td>v0.0.1  <td>aron566 <td>初始版本
 *  </table>
 *  @copyright Copyright (c) 2024  Suzhou Gate-Sea Co.,Ltd.
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "gsmcu_hal.h"
#include "gs2100xx_it.h"
#include "uart.h"
#include "los_config.h"
#include "los_event.h"
#include "circular_queue.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/
#define BPS_UART_OPEN      (1)
#define BPS_UART_OPEN_MASK (1)

#define BPS_UART_HAVEDATA      (0x01 << 4)
#define BPS_UART_HAVEDATA_MASK (0x01 << 4)

#define BPS_UART_TRANSMITING  (0x01 << 8)
#define BPS_UART_TRANSMITMASK (0x01 << 8)

#define BPS_UART_BUFF_LENGTH (2001)

#define Stop_Bits_One (USART_8Bit_StopBits_1)
#define Stop_Bits_Two (USART_8Bit_StopBits_2)

#define Parity_No   (USART_Parity_No)
#define Parity_Even (USART_Parity_Even)
#define Parity_Odd  (USART_Parity_Odd)

#define METER_UART_DMA_CHL DMA_Channel0
#define DEBUG_UART_DMA_CHL DMA_Channel1

#define DEBUG_RX_BUFFER_SIZE_MAX  (uint32_t)CQ_BUF_256B/**< 调试串口缓冲区大小 */
/** Private typedef ----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
#if (LOSCFG_USE_SHELL == 1)
extern EVENT_CB_S g_shellInputEvent;
#endif
/** Private variables --------------------------------------------------------*/
static uint8_t BPS_UartRxBuff[BPS_UART_BUFF_LENGTH];

static uint32_t BPS_UartRxBuffReadIndex;
static uint32_t BPS_UartRxBuffWrtieIndex;

/* 调试串口缓冲区 */
static CQ_handleTypeDef Debug_CQ_Handle;
static uint8_t Debug_CQ_Buff[DEBUG_RX_BUFFER_SIZE_MAX];
/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
 *
 *       Static code
 *
 ********************************************************************************
 */

/**
 * @brief 串口中断处理函数
 *
 */
void DEBUG_UART_Handler_ISR(void)
{
  uint32_t status = USART_GetITAllStatus(DEBUG_UART);
  switch (status)
  {
    case USART_RECEIVE_DATA_READY_INTER_STATUS:
    {
      /* 取出FIFO所有数据 */
      uint8_t data = 0U;
      uint8_t FIFO_Len = USART_GetFifoDepth(DEBUG_UART);
      for (uint8_t i = 0U; i < (FIFO_Len - 1U); i++)
      {
        data = USART_ReceiveData(DEBUG_UART);

        /* 存入环形缓冲区 */
        CQ_putData(&Debug_CQ_Handle, &data, 1U);
      }
      /* 通知接收完成 */
      #if (LOSCFG_USE_SHELL == 1)
      LOS_EventWrite(&g_shellInputEvent, 0x1);
      #endif
    }
    break;

    case USART_RECEIVE_DATA_TIMEOUT_INTER_STATUS:
    {
      uint8_t data = 0U;
      while (USART_GetStatus(DEBUG_UART, USART_DATA_READY_STATUS) == SET)
      {
        data = USART_ReceiveData(DEBUG_UART);

        /* 存入环形缓冲区 */
        CQ_putData(&Debug_CQ_Handle, &data, 1U);
      }

      /* 通知接收完成 */
      #if (LOSCFG_USE_SHELL == 1)
      LOS_EventWrite(&g_shellInputEvent, 0x1);
      #endif
    }
    break;

    case USART_RECEIVE_LINE_INTER_STATUS:
      /* read clear */
      USART_GetAllStatus(DEBUG_UART);

      /* send error occured */
      break;

    case USART_TRANSMITTER_DATA_EMPTY_INTER_STATUS:
      /* 发送完成 */
      break;

    default:
      /* send error occured */
      break;
  }
}

/**
 * @brief 调试串口DMA中断处理
 *
 */
static void DEBUG_UART_DMAC_Handler_IRQ(void)
{
}

void UsartReceiveComplate(void)
{
}

void UsartSendComplate(void)
{
}

/**
 * @brief 串口初始化
 *
 */
void UART_Init(void)
{
  UartClose();
  UartParameterType Usart0Para      = {0};
  Usart0Para.BaudRate               = 115200;
  Usart0Para.DataBits               = 8;
  Usart0Para.Parity                 = Parity_Even;
  Usart0Para.StopBits               = Stop_Bits_One;
  Usart0Para.RxHaveDataFunction     = UsartReceiveComplate;
  Usart0Para.TxDataCompleteFunction = UsartSendComplate;
  Usart0Para.RxParityErrorFunction  = NULL;
  UartOpen(&Usart0Para);

  DebugOpen(115200U);
}
/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */
#if 1
uint32_t IncreaseIndex(uint32_t idx, uint32_t buffsize)
{
  idx++;
  if (idx >= buffsize)
  {
    idx = 0;
  }
  return idx;
}
//
bool IsFifoEmpty(uint32_t *ri, uint32_t *wi)
{
  return (*ri == *wi) ? true : false;
}

bool IsFifoFull(uint32_t *ri, uint32_t *wi, uint32_t buffsize)
{
  return (IncreaseIndex(*wi, buffsize) == *ri) ? true : false;
}

void InitFifo(uint32_t *ri, uint32_t *wi)
{
  *ri = 0;
  *wi = 0;
}

bool PushFifo(uint32_t *ri, uint32_t *wi, uint8_t *pbuff, uint32_t buffsize, uint8_t dv)
{
  uint32_t witmp;
  if (IsFifoFull(ri, wi, buffsize))
    return false;
  witmp        = IncreaseIndex(*wi, buffsize);
  pbuff[witmp] = dv;
  *wi          = witmp;
  return true;
}

bool PopFifo(uint32_t *ri, uint32_t *wi, uint8_t *pbuff, uint32_t buffsize, uint8_t *pdv)
{
  uint32_t ritmp;
  if (IsFifoEmpty(ri, wi))
    return false;
  ritmp = *ri;
  *pdv  = pbuff[ritmp];
  *ri   = IncreaseIndex(ritmp, buffsize);
  return true;
}

uint32_t GetFifoDataLen(uint32_t ri, uint32_t wi, uint32_t buffsize)
{
  uint32_t len;
  if (wi >= ri)
  {
    len = (wi) - (ri);
  }
  else
  {
    len = buffsize - (ri) + (wi);
  }
  return len;
}

uint32_t PopSomeOfFifo(uint32_t *ri, uint32_t *wi, uint8_t *pbuff, uint32_t buffsize, uint8_t *pdst, uint32_t poplen)
{
  static bool InReadFlg = false;
  uint32_t    witmp, ritmp, rilen = 0;

  if (poplen == 0)
    return 0;
  if (false == InReadFlg)
  {
    InReadFlg = true;
    if (IsFifoEmpty(ri, wi))
    {
      InReadFlg = false;
      return 0;
    }
    rilen = 0;
    ritmp = *ri;
    witmp = *wi;
    for (uint32_t i = 0; i < poplen; i++)
    {
      ritmp   = IncreaseIndex(ritmp, buffsize);
      pdst[i] = pbuff[ritmp];
      ++rilen;
      if (ritmp == witmp)
      {
        break;
      }
    }
    *ri       = ritmp;
    InReadFlg = false;
  }
  return rilen;
}

static uint32_t UartStatus    = 0x00;
static uint32_t UartFifoDepth = 0x00;

static UartCallBackFunctionType UartTxCallBackFunction      = NULL;
static UartCallBackFunctionType UartRxDataCallBackFunction  = NULL;
static UartCallBackFunctionType UartRxErrorCallBackFunction = NULL;
//
void METER_UART_Handler_ISR(void);
void DMAC_Handler_METER_UART_ISR(void);
//
void UartOpen(UartParameterType *UartParameter)
{
  #if defined(METER_UART)
  PinRemapConfig(METER_UART_TX_PIN_REMAP, ENABLE);
  PinRemapConfig(METER_UART_RX_PIN_REMAP, ENABLE);
  SCU_PeriphInputConfig(METER_UART_INPUT_ENABLE);
  SCU_GPIOInputConfig(METER_UART_TX_PORT, METER_UART_TX_PIN);
  SCU_GPIOInputConfig(METER_UART_RX_PORT, METER_UART_RX_PIN);
  SCU_SetUsartMode(METER_UART, USART_DONT_INVERT38K_OUTPUT |
    #if defined(METER_UART_OPEN_DRAIN)
                                 USART_OPEN_DRAIN_ENABLE |
    #else
                                 USART_OPEN_DRAIN_DISABLE |
    #endif
                                 USART_SELECT_NORMAL__USART_MODE | USART_IO_FAST | USART_IO_DRIVER_12MA);
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = METER_UART_RX_PIN;
    #if DEV_CCO
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    #else
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    #endif
  GPIO_Init(METER_UART_RX_PORT, &GPIO_InitStruct);

  USART_InitTypeDef USART_InitStruct;

  USART_InitStruct.USART_BaudRate            = UartParameter->BaudRate;
  USART_InitStruct.USART_WordLength_StopBits = UartParameter->StopBits;
  USART_InitStruct.USART_Parity              = UartParameter->Parity;
  //
  UartTxCallBackFunction      = UartParameter->TxDataCompleteFunction;
  UartRxDataCallBackFunction  = UartParameter->RxHaveDataFunction;
  UartRxErrorCallBackFunction = UartParameter->RxParityErrorFunction;
  //
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Fifo_Config         = USART_RXFIFO_THR_LEVEL_3 | USART_TXFIFO_THR_LEVEL_0 |
                                       USART_RXFIFO_CLEAR | USART_TXFIFO_CLEAR | USART_FIFO_ENABLE;   // use fifo
  USART_InitStruct.USART_DMA_Flag = ENABLE;
  // USART_InitStruct.USART_Fifo_Config = USART_FIFO_CONFIG_NONE;//not use fifo

  //
  USART_Init(METER_UART, &USART_InitStruct);
  UartFifoDepth = USART_GetFifoDepth(METER_UART);
  //
  UartStatus = BPS_UART_OPEN;
  InitFifo(&BPS_UartRxBuffReadIndex, &BPS_UartRxBuffWrtieIndex);
  /* enable interrupt */
  USART_ConfigInterruptEnable(METER_UART, USART_RECEIVE_LINE_INTER_ENABLE | USART_RECEIVE_DATA_INTER_ENABLE);
  //
  // BSP_IntVectSet(METER_UART_IRQn, METER_UART_Handler_ISR);
  NVIC_SetVector(METER_UART_IRQn, (uint32_t)METER_UART_Handler_ISR);
  /* DMA */
  // BSP_InitDmacVector();
  ReSet_DMA_Channel(DMA, METER_UART_DMA_CHL);

  /* 设置串口dma通道中断 */
  GS2100xx_DMA_IT_Handler_Set((int)METER_UART_DMA_CHL, DMAC_Handler_METER_UART_ISR);

  NVIC_ClearPendingIRQ(METER_UART_IRQn);
  NVIC_EnableIRQ(METER_UART_IRQn);
  #endif
}

/**
 * @brief 调试串口打开
 *
 * @param BaudRate 波特率
 */
void DebugOpen(uint32_t BaudRate)
{
  #if defined(DEBUG_UART)
  /* 初始化环形缓冲区 */
  CQ_init(&Debug_CQ_Handle, Debug_CQ_Buff, DEBUG_RX_BUFFER_SIZE_MAX);

  PinRemapConfig(DEBUG_UART_TX_PIN_REMAP, ENABLE);
  PinRemapConfig(DEBUG_UART_RX_PIN_REMAP, ENABLE);
  SCU_PeriphInputConfig(DEBUG_UART_INPUT_ENABLE);
  SCU_GPIOInputConfig(DEBUG_UART_TX_PORT, DEBUG_UART_TX_PIN);
  SCU_GPIOInputConfig(DEBUG_UART_RX_PORT, DEBUG_UART_RX_PIN);
  SCU_SetUsartMode(DEBUG_UART, USART_DONT_INVERT38K_OUTPUT | USART_OPEN_DRAIN_DISABLE | USART_SELECT_NORMAL__USART_MODE | USART_IO_FAST | USART_IO_DRIVER_12MA);

  /* GPIO引脚配置 */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin  = DEBUG_UART_RX_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(DEBUG_UART_RX_PORT, &GPIO_InitStruct);

  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate            = BaudRate;
  USART_InitStruct.USART_WordLength_StopBits = USART_8Bit_StopBits_1;
  USART_InitStruct.USART_Parity              = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Fifo_Config         = USART_RXFIFO_THR_LEVEL_3 | USART_TXFIFO_THR_LEVEL_0 |
                                       USART_RXFIFO_CLEAR | USART_TXFIFO_CLEAR | USART_FIFO_ENABLE;   // use fifo
  // USART_InitStruct.USART_Fifo_Config = USART_FIFO_CONFIG_NONE; //not use fifo
  USART_InitStruct.USART_DMA_Flag = ENABLE;
  USART_Init(DEBUG_UART, &USART_InitStruct);

  ReSet_DMA_Channel(DMA, DEBUG_UART_DMA_CHL);

  /* 设置串口dma通道中断 */
  GS2100xx_DMA_IT_Handler_Set((int)DEBUG_UART_DMA_CHL, DEBUG_UART_DMAC_Handler_IRQ);

  /* enable interrupt */
  USART_ConfigInterruptEnable(DEBUG_UART, USART_RECEIVE_LINE_INTER_ENABLE | USART_RECEIVE_DATA_INTER_ENABLE);

  /* set interrupt */
  NVIC_SetVector(DEBUG_UART_IRQn, (uint32_t)DEBUG_UART_Handler_ISR);

  /* enable interrupt */
  NVIC_ClearPendingIRQ(DEBUG_UART_IRQn);
  NVIC_EnableIRQ(DEBUG_UART_IRQn);

  printf("Debug Is Opened.\r\n");
  #endif
}

void UartWriteWithDma(USART_TypeDef *puart, uint8_t *pbuff, uint32_t len, uint32_t dma_channel)
{
  // DMA
  DMA_InitTypeDef DMA_InitStruct;
  if (len < 1)
    return;
  DMA_InitStruct.CSR = CSR_SRC_SIZE_1 |
                       CSR_SRC_WIDTH_8BIT |
                       CSR_DST_WIDTH_8BIT |
                       CSR_MODE_HANDSHAKE |
                       CSR_SRCAD_CTL_INC |
                       CSR_DSTAD_CTL_FIX;

  DMA_InitStruct.CFG = CFG_DST_HE_SET | CFG_SRC_HE_SET |
                       CFG_INT_ABT_MSK | CFG_INT_ERR_MSK;   //|CFG_INT_TC_MSK;

  DMA_InitStruct.SrcAdd  = (uint32_t)pbuff;
  DMA_InitStruct.DstAddr = GetUartSRAddr(puart);
  DMA_InitStruct.LLP     = 0;
  if (GetUartDmaSrc(puart, &DMA_InitStruct.ChlPeriph, NULL) != 0)
  {
    __BKPT(1);
  }
  DMA_InitStruct.SIZE     = len;
  DMA_InitStruct.SYNC_Flg = SET;
  DMA_Init(DMA, dma_channel, &DMA_InitStruct);
  DMA_ClearChannelTCITStatus(DMA, dma_channel);
  Set_DMA_Channel(DMA, dma_channel);
}

void UartWrite(uint8_t *pbuff, uint32_t len)
{
  #if defined(METER_UART) && defined(METER_UART_DMA_CHL)
  // DMA
  DMA_InitTypeDef DMA_InitStruct;
  if (len < 1)
    return;
  if ((UartStatus & BPS_UART_OPEN_MASK) != BPS_UART_OPEN)
    return;
  DMA_InitStruct.CSR = CSR_SRC_SIZE_1 |
                       CSR_SRC_WIDTH_8BIT |
                       CSR_DST_WIDTH_8BIT |
                       CSR_MODE_HANDSHAKE |
                       CSR_SRCAD_CTL_INC |
                       CSR_DSTAD_CTL_FIX;

  DMA_InitStruct.CFG = CFG_DST_HE_SET | CFG_SRC_HE_SET |
                       CFG_INT_ABT_MSK | CFG_INT_ERR_MSK;   //|CFG_INT_TC_MSK;

  DMA_InitStruct.SrcAdd    = (uint32_t)pbuff;
  DMA_InitStruct.DstAddr   = GetUartSRAddr(METER_UART);
  DMA_InitStruct.LLP       = 0;
  DMA_InitStruct.ChlPeriph = METER_UART_DMA_REQ;
  DMA_InitStruct.SIZE      = len;
  DMA_InitStruct.SYNC_Flg  = SET;
  DMA_Init(DMA, METER_UART_DMA_CHL, &DMA_InitStruct);
  //
  DMA_ClearChannelTCITStatus(DMA, METER_UART_DMA_CHL);
  //
  Set_DMA_Channel(DMA, METER_UART_DMA_CHL);
  //
  UartStatus |= BPS_UART_TRANSMITING;
  #endif
}
uint32_t UartRead(uint8_t *pbuff, uint32_t len)
{
  #if defined(METER_UART)
  if ((UartStatus & BPS_UART_OPEN_MASK) != BPS_UART_OPEN)
    return 0;
  if (len == 0)
  {
    len = BPS_UART_BUFF_LENGTH;
  }
  return PopSomeOfFifo(&BPS_UartRxBuffReadIndex, &BPS_UartRxBuffWrtieIndex, BPS_UartRxBuff, BPS_UART_BUFF_LENGTH, pbuff, len);
  #else
  return -1;
  #endif
}

uint32_t GetUartRxDataLen()
{
  #if defined(METER_UART)
  return GetFifoDataLen(BPS_UartRxBuffReadIndex, BPS_UartRxBuffWrtieIndex, BPS_UART_BUFF_LENGTH);
  #else
  return -1;
  #endif
}

void UartClose()
{
  #if defined(METER_UART) && defined(METER_UART_DMA_CHL)
  if ((UartStatus & BPS_UART_OPEN_MASK) != BPS_UART_OPEN)
    return;
  NVIC_DisableIRQ(METER_UART_IRQn);
  ReSet_DMA_Channel(DMA, METER_UART_DMA_CHL);
  UartStatus = 0x00;
  #endif
}

bool IsUartTxData()
{
  #if defined(METER_UART)
  if ((UartStatus & BPS_UART_OPEN_MASK) != BPS_UART_OPEN)
    return false;
  if ((UartStatus & BPS_UART_TRANSMITMASK) == BPS_UART_TRANSMITING)
    return true;
  #endif
  return false;
}

bool IsUartOpen()
{
  #if defined(METER_UART)
  if ((UartStatus & BPS_UART_OPEN_MASK) != BPS_UART_OPEN)
    return false;
  return true;
  #else
  return false;
  #endif
}

// extern void UART_ExernExe(void);
void METER_UART_Handler_ISR(void)
{
  // UART_ExernExe();
  #if defined(METER_UART)
  switch (USART_GetITAllStatus(METER_UART))
  {
    case USART_RECEIVE_DATA_READY_INTER_STATUS:
    {
      for (int i = 0; i < (UartFifoDepth - 1); i++)
      {
        uint8_t dtmp = USART_ReceiveData(METER_UART);

        if (false == PushFifo(&BPS_UartRxBuffReadIndex, &BPS_UartRxBuffWrtieIndex, BPS_UartRxBuff, BPS_UART_BUFF_LENGTH, dtmp))
        {
          if (UartRxDataCallBackFunction != NULL)
          {
            UartRxDataCallBackFunction();
            PushFifo(&BPS_UartRxBuffReadIndex, &BPS_UartRxBuffWrtieIndex, BPS_UartRxBuff, BPS_UART_BUFF_LENGTH, dtmp);
          }
        }
      }
    }
    break;
    case USART_RECEIVE_DATA_TIMEOUT_INTER_STATUS:
    {
      uint8_t dtmp;
      while (USART_GetStatus(METER_UART, USART_DATA_READY_STATUS) == SET)
      {
        dtmp = USART_ReceiveData(METER_UART);
        if (false == PushFifo(&BPS_UartRxBuffReadIndex, &BPS_UartRxBuffWrtieIndex, BPS_UartRxBuff, BPS_UART_BUFF_LENGTH, dtmp))
        {
          if (UartRxDataCallBackFunction != NULL)
          {
            UartRxDataCallBackFunction();
            PushFifo(&BPS_UartRxBuffReadIndex, &BPS_UartRxBuffWrtieIndex, BPS_UartRxBuff, BPS_UART_BUFF_LENGTH, dtmp);
          }
        }
      }
      if (UartRxDataCallBackFunction != NULL)
      {
        UartRxDataCallBackFunction();
      }
    }
    break;
    case USART_RECEIVE_LINE_INTER_STATUS:
      USART_GetAllStatus(METER_UART);   // read clear
      if (UartRxErrorCallBackFunction != NULL)
      {
        UartRxErrorCallBackFunction();
      }
      break;
    case USART_TRANSMITTER_DATA_EMPTY_INTER_STATUS:
      break;
    default:
      // error
      break;
  }
  #endif
}

#endif

void DMAC_Handler_METER_UART_ISR(void)
{
#if defined(METER_UART)
  UartStatus &= ~BPS_UART_TRANSMITMASK;
  if (UartTxCallBackFunction != NULL)
  {
    UartTxCallBackFunction();
  }
#endif
}

/**
 * @brief 获取串口数据 for shell
 *
 * @return uint8_t 数据
 */
uint8_t UartGetc(void)
{
  uint8_t data = 0;
  CQ_getData(&Debug_CQ_Handle, &data, 1U);
  return data;
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/
