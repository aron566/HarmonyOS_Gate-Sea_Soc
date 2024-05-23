/**
 *  @file circular_queue.c
 *
 *  @date 2023年07月20日 11:02:13 星期四
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2023 aron566 <aron566@163.com>.
 *
 *  @brief 环形缓冲区.
 *
 *  @details None.
 *
 *  @version v0.0.1 aron566 2023.07.20 11:02 初始版本.
 *
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version <th>Author  <th>Description
 *  <tr><td>2023-07-20 <td>v0.0.1  <td>aron566 <td>初始版本
 *  <tr><td>2024-05-22 <td>v0.0.2  <td>aron566 <td>适配鸿蒙内核
 *  </table>
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "circular_queue.h"

/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/

/**
 * @brief 比较两个数，获取最小值
 *
 */
#define GET_MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * @brief 判断这个数是否为2的幂次方
 *
 */
#define IS_POWER_OF_2(x) ((x) != 0U && (((x) & ((x) - 1U)) == 0U))

/* 区域指定 */
#define AT_CQ_CODE_SPACE  //MATH_PORT_SECTION("USE_ITCM_SPACE")//(".text.aidis.cq")
#define AT_CQ_RAM_SPACE   //MATH_PORT_SECTION("USE_DTCM_SPACE")//(".aids.cq.cache")
#define AT_CQ_RO_SPACE    //MATH_PORT_SECTION("USE_ITCM_SPACE")//(".text.aidis.cq.ro")

#define USE_ARM_DSP_FUNC_EN   0/**< 使能DSP加速 */

#if USE_ARM_DSP_FUNC_EN
  #include "arm_math.h"
#endif
/** Private typedef ----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/


/**
 * [CQ_init 环形缓冲区初始化]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  memAdd        [数据存储区]
 * @param  len           [缓冲区大小]
 * @return               [初始化成功状态]
 */
AT_CQ_CODE_SPACE
bool CQ_init(CQ_handleTypeDef *CircularQueue, uint8_t *memAdd, uint32_t len)
{
  CircularQueue->size = len;

  if(!IS_POWER_OF_2(CircularQueue->size))
  {
    return false;
  }

  if(NULL == memAdd)
  {
    return false;
  }

  CircularQueue->Buffer.data8Buffer = memAdd;

  (void)memset(CircularQueue->Buffer.data8Buffer, 0, len);
  CircularQueue->exit = 0U;
  CircularQueue->entrance = 0U;
  return true;
}

/**
 * [CQ_isEmpty 环形缓冲区判断是否为空]
 * @param  CircularQueue [环形缓冲区句柄]
 * @return               [true 为空]
 */
AT_CQ_CODE_SPACE
bool CQ_isEmpty(CQ_handleTypeDef *CircularQueue)
{
  if(CircularQueue->entrance == CircularQueue->exit)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * [CQ_isFull 环形缓冲区判断是否为满]
 * @param  CircularQueue [环形缓冲区句柄]
 * @return               [true 为满]
 */
AT_CQ_CODE_SPACE
bool CQ_isFull(CQ_handleTypeDef *CircularQueue)
{
  if((CircularQueue->entrance - CircularQueue->exit) == CircularQueue->size)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * [CQ_getLength 环形缓冲区获取剩余空间长度]
 * @param  CircularQueue [环形缓冲区句柄]
 * @return               [剩余长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_getLength(CQ_handleTypeDef *CircularQueue)
{
  return (CircularQueue->entrance - CircularQueue->exit) > CircularQueue->size?CircularQueue->size:(CircularQueue->entrance - CircularQueue->exit);
}

/**
 * [CQ_emptyData 环形缓冲区清空操作]
 * @param  CircularQueue [环形缓冲区句柄]
 * @return               [None]
 */
AT_CQ_CODE_SPACE
void CQ_emptyData(CQ_handleTypeDef *CircularQueue)
{
  CircularQueue->exit = 0U;
  CircularQueue->entrance = 0U;
  (void)memset(CircularQueue->Buffer.data8Buffer, 0, CircularQueue->size);
}

/**
 * [CQ_getData 环形缓冲区读走数据]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  targetBuf     [目标缓冲区]
 * @return               [读取的长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_getData(CQ_handleTypeDef *CircularQueue, uint8_t *targetBuf, uint32_t len)
{
  uint32_t size = 0;

  len = GET_MIN(len, CircularQueue->entrance - CircularQueue->exit);

  size = GET_MIN(len, CircularQueue->size - (CircularQueue->exit & (CircularQueue->size - 1U)));

  /* 使用DSP加速 */
#if USE_ARM_DSP_FUNC_EN
  arm_copy_q7((q7_t *)(CircularQueue->Buffer.data8Buffer + (CircularQueue->exit & (CircularQueue->size - 1U))), (q7_t *)targetBuf, size);
  arm_copy_q7((q7_t *)CircularQueue->Buffer.data8Buffer, (q7_t *)(targetBuf + size), len - size);
#else
  (void)memcpy(targetBuf, &CircularQueue->Buffer.data8Buffer[(CircularQueue->exit & (CircularQueue->size - 1U))], size);
  (void)memcpy(targetBuf + size, CircularQueue->Buffer.data8Buffer, len - size);
#endif

  /* 利用无符号数据的溢出特性 */
  CircularQueue->exit += len;

  return len;
}


/**
 * [CQ_putData 环形缓冲区加入新数据]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  sourceBuf     [为实际存储区地址]
 * @param  len           [数据存入长度]
 * @return               [存入的长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_putData(CQ_handleTypeDef *CircularQueue, const uint8_t *sourceBuf, uint32_t len)
{
  uint32_t size = 0;
  /* 此次存入的实际大小，取 剩余空间 和 目标存入数量  两个值小的那个 */
  len = GET_MIN(len, CircularQueue->size - CircularQueue->entrance + CircularQueue->exit);

  /* &(size-1)代替取模运算，同上原理，得到此次存入队列入口到末尾的大小 */
  size = GET_MIN(len, CircularQueue->size - (CircularQueue->entrance & (CircularQueue->size - 1U)));

  /* 使用DSP加速 */
#if USE_ARM_DSP_FUNC_EN
  arm_copy_q7((q7_t *)sourceBuf, (q7_t *)(CircularQueue->Buffer.data8Buffer + (CircularQueue->entrance & (CircularQueue->size - 1U))), size);
  arm_copy_q7((q7_t *)(sourceBuf + size), (q7_t *)(CircularQueue->Buffer.data8Buffer), len - size);
#else
  (void)memcpy(&CircularQueue->Buffer.data8Buffer[(CircularQueue->entrance & (CircularQueue->size - 1U))], sourceBuf, size);
  (void)memcpy(CircularQueue->Buffer.data8Buffer, sourceBuf + size, len - size);
#endif

  /* 利用无符号数据的溢出特性 */
  CircularQueue->entrance += len;

  return len;
}

/**
  ******************************************************************
  * @brief   跳过8位无效帧头数据
  * @param   [in]CircularQueue 缓冲区
  * @param   [in]header_data 无效数据
  * @return  缓冲区可读长度
  * @author  aron566
  * @version V1.0
  * @date    2020-09-20
  ******************************************************************
  */
AT_CQ_CODE_SPACE
uint32_t CQ_skipInvaildU8Header(CQ_handleTypeDef *CircularQueue, uint8_t Data)
{
  uint8_t header = 0;
  uint32_t Size = CQ_getLength(CircularQueue);
  for(uint32_t i = 0U; i < Size; i++)
  {
    if(0U == CQ_getLength(CircularQueue))
    {
      return 0;
    }
    header = CQ_ManualGet_Offset_Data(CircularQueue, 0);
    /* 判断帧头 */
    if(header != Data)
    {
      CQ_ManualOffsetInc(CircularQueue, 1U);
    }
    else
    {
      return CQ_getLength(CircularQueue);
    }
  }
  return 0;
}

/**
  ******************************************************************
  * @brief   跳过16位无效帧头数据
  * @param   [in]CircularQueue 缓冲区
  * @param   [in]header_data 无效数据
  * @return  缓冲区可读长度
  * @author  aron566
  * @version V1.0
  * @date    2020-09-20
  ******************************************************************
  */
AT_CQ_CODE_SPACE
uint32_t CQ_skipInvaildU16Header(CQ_handleTypeDef *CircularQueue, uint16_t Data)
{
  uint16_t header = 0;
  uint32_t Size = CQ_getLength(CircularQueue);
  for(uint32_t i = 0U; i < Size; i++)
  {
    if(2U > CQ_getLength(CircularQueue))
    {
      return CQ_getLength(CircularQueue);
    }
    header = CQ_ManualGet_Offset_Data(CircularQueue, 0);
    header |= (((uint16_t)CQ_ManualGet_Offset_Data(CircularQueue, 1)) << 8U);
    /* 判断帧头 */
    if(header != Data)
    {
      CQ_ManualOffsetInc(CircularQueue, 1U);
    }
    else
    {
      return CQ_getLength(CircularQueue);
    }
  }
  return CQ_getLength(CircularQueue);
}

/**
  ******************************************************************
  * @brief   跳过32位无效帧头数据
  * @param   [in]CircularQueue 缓冲区
  * @param   [in]header_data 无效数据
  * @return  缓冲区可读长度
  * @author  aron566
  * @version V1.0
  * @date    2020-09-20
  ******************************************************************
  */
AT_CQ_CODE_SPACE
uint32_t CQ_skipInvaildU32Header(CQ_handleTypeDef *CircularQueue, uint32_t Data)
{
  uint32_t header = 0;
  uint32_t Size = CQ_getLength(CircularQueue);
  for(uint32_t i = 0U; i < Size; i++)
  {
    if(4U > CQ_getLength(CircularQueue))
    {
      return CQ_getLength(CircularQueue);
    }
    header = CQ_ManualGet_Offset_Data(CircularQueue, 0);
    header |= (((uint32_t)CQ_ManualGet_Offset_Data(CircularQueue, 1)) << 8U);
    header |= (((uint32_t)CQ_ManualGet_Offset_Data(CircularQueue, 2)) << 16U);
    header |= (((uint32_t)CQ_ManualGet_Offset_Data(CircularQueue, 3)) << 24U);
    /* 判断帧头 */
    if(header != Data)
    {
      CQ_ManualOffsetInc(CircularQueue, 1U);
    }
    else
    {
      return CQ_getLength(CircularQueue);
    }
  }
  return CQ_getLength(CircularQueue);
}

/**
  ******************************************************************
  * @brief   跳过Modbus16位无效帧头数据
  * @param   [in]CircularQueue 缓冲区
  * @param   [in]header_data 无效数据
  * @return  缓冲区可读长度
  * @author  aron566
  * @version V1.0
  * @date    2020-09-20
  ******************************************************************
  */
AT_CQ_CODE_SPACE
uint32_t CQ_skipInvaildModbusU16Header(CQ_handleTypeDef *CircularQueue, uint16_t Data)
{
  uint16_t header = 0;
  uint32_t Size = CQ_getLength(CircularQueue);
  for(uint32_t i = 0U; i < Size; i++)
  {
    if(2U > CQ_getLength(CircularQueue))
    {
      return CQ_getLength(CircularQueue);
    }
    header = (((uint16_t)CQ_ManualGet_Offset_Data(CircularQueue, 0)) << 8U);
    header |= CQ_ManualGet_Offset_Data(CircularQueue, 1);
    /* 判断帧头 */
    if(header != Data)
    {
      CQ_ManualOffsetInc(CircularQueue, 1U);
    }
    else
    {
      return CQ_getLength(CircularQueue);
    }
  }
  return CQ_getLength(CircularQueue);
}

/**
  ******************************************************************
  * @brief   跳过Modbus32位无效帧头数据
  * @param   [in]CircularQueue 缓冲区
  * @param   [in]header_data 无效数据
  * @return  缓冲区可读长度
  * @author  aron566
  * @version V1.0
  * @date    2020-09-20
  ******************************************************************
  */
AT_CQ_CODE_SPACE
uint32_t CQ_skipInvaildModbusU32Header(CQ_handleTypeDef *CircularQueue, uint32_t Data)
{
  uint32_t header = 0;
  uint32_t Size = CQ_getLength(CircularQueue);
  for(uint32_t i = 0U; i < Size; i++)
  {
    if(4U > CQ_getLength(CircularQueue))
    {
      return CQ_getLength(CircularQueue);
    }
    header = (((uint32_t)CQ_ManualGet_Offset_Data(CircularQueue, 0)) << 24U);
    header |= (((uint32_t)CQ_ManualGet_Offset_Data(CircularQueue, 1)) << 16U);
    header |= (((uint32_t)CQ_ManualGet_Offset_Data(CircularQueue, 2)) << 8U);
    header |= CQ_ManualGet_Offset_Data(CircularQueue, 3U);
    /* 判断帧头 */
    if(header != Data)
    {
      CQ_ManualOffsetInc(CircularQueue, 1U);
    }
    else
    {
      return CQ_getLength(CircularQueue);
    }
  }
  return CQ_getLength(CircularQueue);
}

/**
 * [CQ_ManualGetData 临时取出环形队列数据，为了分步取出完整数据]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  targetBuf     [目标缓冲区]
 * @param  len           [数据读取长度]
 * @return               [读取的长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_ManualGetData(CQ_handleTypeDef *CircularQueue, uint8_t *targetBuf, uint32_t len)
{
  uint32_t size = 0;

  /* 此次读取的实际大小，取 可读 和 目标读取数量  两个值小的那个 */
  len = GET_MIN(len, CircularQueue->entrance - CircularQueue->exit);
  /* 原理雷同存入 */
  size = GET_MIN(len, CircularQueue->size - (CircularQueue->exit & (CircularQueue->size - 1U)));

  /* 使用DSP加速 */
#if USE_ARM_DSP_FUNC_EN
  arm_copy_q7((q7_t *)(CircularQueue->Buffer.data8Buffer + (CircularQueue->exit & (CircularQueue->size - 1))), (q7_t *)targetBuf, size);
  arm_copy_q7((q7_t *)(CircularQueue->Buffer.data8Buffer), (q7_t *)(targetBuf + size), len - size);
#else
  memcpy(targetBuf, CircularQueue->Buffer.data8Buffer + (CircularQueue->exit & (CircularQueue->size - 1)), size);
  memcpy(targetBuf + size, CircularQueue->Buffer.data8Buffer, len - size);
#endif

  return len;
}

/**
 * [CQ_ManualGet_Offset_Data 读取指定索引号的数据]
 * @param CircularQueue [环形缓冲区句柄]
 * @param index         [索引号]
 */
AT_CQ_CODE_SPACE
uint8_t CQ_ManualGet_Offset_Data(CQ_handleTypeDef *CircularQueue, uint32_t index)
{
  /* 计算偏移 */
  uint32_t read_offset = ((CircularQueue->exit + index) & (CircularQueue->size - 1U));
  /* 取出数据 */
  uint8_t data = *((uint8_t *)CircularQueue->Buffer.data8Buffer + read_offset);

  return data;
}

/**
 * [CQ_ManualOffsetInc 手动增加已取出长度]
 * @param CircularQueue [环形缓冲区句柄]
 * @param len           [偏移长度]
 */
AT_CQ_CODE_SPACE
void CQ_ManualOffsetInc(CQ_handleTypeDef *CircularQueue, uint32_t len)
{
  len = GET_MIN(CQ_getLength(CircularQueue), len);
  CircularQueue->exit += len;
}

/**
 * [cb_create 申请并初始化环形缓冲区]
 * @param  buffsize [申请环形缓冲区大小]
 * @return          [环形队列管理句柄]
 */
CQ_handleTypeDef *CQ_Create(uint32_t buffSize)
{
  if(!IS_POWER_OF_2(buffSize))
  {
    return NULL;
  }

  CQ_handleTypeDef *CircularQueue = (CQ_handleTypeDef *)OsalMemCalloc(sizeof(CQ_handleTypeDef));
  if(NULL == CircularQueue)
  {
    return NULL;
  }

  CircularQueue->size = buffSize;
  CircularQueue->exit = 0;
  CircularQueue->entrance = 0;
  //the buff never release!
  CircularQueue->Buffer.data8Buffer = (uint8_t *)OsalMemCalloc((size_t)CircularQueue->size);
  if(NULL == CircularQueue->Buffer.data8Buffer)
  {
    return NULL;
  }
  CircularQueue->is_malloc = true;
  return CircularQueue;
}

/**
 * @brief 删除一个缓冲区
 *
 * @param CircularQueue
 */
void CQ_Delete(CQ_handleTypeDef *CircularQueue)
{
  if(NULL == CircularQueue)
  {
    return;
  }
  if(false == CircularQueue->is_malloc)
  {
    return;
  }
  OsalMemFree(CircularQueue->Buffer.data8Buffer);
  OsalMemFree(CircularQueue);
}

/**
 * [CQ_16_init 静态初始化16bit环形缓冲区]
 * @param  CircularQueue [缓冲区指针]
 * @param  memAdd        [uint16_t 缓冲区地址]
 * @param  len           [缓冲区长度>1]
 * @return               [初始化状态]
 */
AT_CQ_CODE_SPACE
bool CQ_16_init(CQ_handleTypeDef *CircularQueue, uint16_t *memAdd, uint32_t len)
{
  CircularQueue->size = len;

  if(!IS_POWER_OF_2(CircularQueue->size))
  {
    return false;
  }

  if(NULL == memAdd)
  {
    return false;
  }

  CircularQueue->Buffer.data16Buffer = memAdd;
  CircularQueue->exit = 0U;
  CircularQueue->entrance = 0U;
  return true;
}

/**
 * [cb_16create 动态申请并初始化环形缓冲区]
 * @param  buffsize [申请环形缓冲区大小]
 * @return          [环形队列管理句柄]
 */
CQ_handleTypeDef *CQ_16Create(uint32_t buffSize)
{
  if(!IS_POWER_OF_2(buffSize))
  {
    return NULL;
  }

  CQ_handleTypeDef *CircularQueue = (CQ_handleTypeDef *)OsalMemCalloc(sizeof(CQ_handleTypeDef));
  if(NULL == CircularQueue)
  {
    return NULL;
  }

  CircularQueue->size = buffSize;
  CircularQueue->exit = 0;
  CircularQueue->entrance = 0;

  CircularQueue->Buffer.data16Buffer = (uint16_t *)OsalMemAllocAlign(sizeof(uint16_t), (size_t)CircularQueue->size * sizeof(uint16_t));
  if(NULL == CircularQueue->Buffer.data16Buffer)
  {
    return NULL;
  }
  CircularQueue->is_malloc = true;
  return CircularQueue;
}

/**
 * [CQ_16getData 取出数据]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  targetBuf     [目标地址]
 * @param  len           [取出长度]
 * @return               [取出长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_16getData(CQ_handleTypeDef *CircularQueue, uint16_t *targetBuf, uint32_t len)
{
  uint32_t size = 0;

  /* 此次读取的实际大小，取 可读 和 目标读取数量  两个值小的那个 */
  len = GET_MIN(len, CircularQueue->entrance - CircularQueue->exit);
  /* 原理雷同存入 */
  size = GET_MIN(len, CircularQueue->size - (CircularQueue->exit & (CircularQueue->size - 1U)));

  /* 使用DSP加速 */
#if USE_ARM_DSP_FUNC_EN
  arm_copy_q15((q15_t *)(CircularQueue->Buffer.data16Buffer + (CircularQueue->exit & (CircularQueue->size - 1U))), (q15_t *)targetBuf, size);
  arm_copy_q15((q15_t *)(CircularQueue->Buffer.data16Buffer), (q15_t *)(targetBuf + size), len - size);
#else
  uint32_t len_temp  = 2U * len;
  uint32_t size_temp = 2U * size;
  (void)memcpy(targetBuf, &CircularQueue->Buffer.data16Buffer[(CircularQueue->exit & (CircularQueue->size - 1U))], size_temp);
  (void)memcpy(targetBuf + size, CircularQueue->Buffer.data16Buffer, len_temp - size_temp);
#endif

  CircularQueue->exit += len;
  return len;
}


/**
 * [CQ_16putData 加入数据]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  sourceBuf     [源地址]
 * @param  len           [长度]
 * @return               [加入数据长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_16putData(CQ_handleTypeDef *CircularQueue, const uint16_t *sourceBuf, uint32_t len)
{
  uint32_t size = 0;

  /* 此次存入的实际大小，取 剩余空间 和 目标存入数量  两个值小的那个 */
  len = GET_MIN(len, CircularQueue->size - CircularQueue->entrance + CircularQueue->exit);

  /* &(size-1)代替取模运算，同上原理，得到此次存入队列入口到末尾的大小 */
  size = GET_MIN(len, CircularQueue->size - (CircularQueue->entrance & (CircularQueue->size - 1U)));

  /* 使用DSP加速 */
#if USE_ARM_DSP_FUNC_EN
  arm_copy_q15((q15_t *)sourceBuf, (q15_t *)(CircularQueue->Buffer.data16Buffer + (CircularQueue->entrance & (CircularQueue->size - 1U))), size);
  arm_copy_q15((q15_t *)(sourceBuf + size), (q15_t *)CircularQueue->Buffer.data16Buffer, len - size);
#else
  uint32_t len_temp  = 2U * len;
  uint32_t size_temp = 2U * size;
  (void)memcpy(&CircularQueue->Buffer.data16Buffer[(CircularQueue->entrance & (CircularQueue->size - 1U))], sourceBuf, size_temp);
  (void)memcpy(CircularQueue->Buffer.data16Buffer, sourceBuf + size, len_temp - size_temp);
#endif

  CircularQueue->entrance += len;
  return len;
}

/**
 * [CQ_32_init 静态初始化32bit环形缓冲区]
 * @param  CircularQueue [缓冲区指针]
 * @param  memAdd        [uint32_t 缓冲区地址]
 * @param  len           [缓冲区长度>1]
 * @return               [初始化状态]
 */
AT_CQ_CODE_SPACE
bool CQ_32_init(CQ_handleTypeDef *CircularQueue, uint32_t *memAdd, uint32_t len)
{
  CircularQueue->size = len;

  if(!IS_POWER_OF_2(CircularQueue->size))
  {
    return false;
  }

  if(memAdd == NULL)
  {
    return false;
  }

  CircularQueue->Buffer.data32Buffer = memAdd;
  CircularQueue->exit = 0U;
  CircularQueue->entrance = 0U;
  return true;
}

/**
 * [cb_32create 动态申请并初始化环形缓冲区]
 * @param  buffsize [申请环形缓冲区大小]
 * @return          [环形队列管理句柄]
 */
CQ_handleTypeDef *CQ_32Create(uint32_t buffSize)
{
  if(!IS_POWER_OF_2(buffSize))
  {
    return NULL;
  }

  CQ_handleTypeDef *CircularQueue = (CQ_handleTypeDef *)OsalMemCalloc(sizeof(CQ_handleTypeDef));
  if(NULL == CircularQueue)
  {
    return NULL;
  }

  CircularQueue->size = buffSize;
  CircularQueue->exit = 0;
  CircularQueue->entrance = 0;

  CircularQueue->Buffer.data32Buffer = (uint32_t *)OsalMemAllocAlign(sizeof(uint32_t), (size_t)CircularQueue->size * sizeof(uint32_t));
  if(NULL == CircularQueue->Buffer.data32Buffer)
  {
    return NULL;
  }
  CircularQueue->is_malloc = true;
  return CircularQueue;
}

/**
 * [CQ_32putData 加入数据]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  sourceBuf     [源地址]
 * @param  len           [长度]
 * @return               [加入数据长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_32putData(CQ_handleTypeDef *CircularQueue, const uint32_t * sourceBuf, uint32_t len)
{
  uint32_t size = 0;

  /* 此次存入的实际大小，取 剩余空间 和 目标存入数量  两个值小的那个 */
  len = GET_MIN(len, CircularQueue->size - CircularQueue->entrance + CircularQueue->exit);

  /* &(size-1)代替取模运算，同上原理，得到此次存入队列入口到末尾的大小 */
  size = GET_MIN(len, CircularQueue->size - (CircularQueue->entrance & (CircularQueue->size - 1U)));

  /* 使用DSP加速 */
#if USE_ARM_DSP_FUNC_EN
  arm_copy_q31((q31_t *)sourceBuf, (q31_t *)(CircularQueue->Buffer.data32Buffer + (CircularQueue->entrance & (CircularQueue->size - 1U))), size);
  arm_copy_q31((q31_t *)(sourceBuf + size), (q31_t *)(CircularQueue->Buffer.data32Buffer), len - size);
#else
  uint32_t len_temp  = 4U * len;
  uint32_t size_temp = 4U * size;
  (void)memcpy(&CircularQueue->Buffer.data32Buffer[(CircularQueue->entrance & (CircularQueue->size - 1U))], sourceBuf, size_temp);
  (void)memcpy(CircularQueue->Buffer.data32Buffer, sourceBuf + size, len_temp - size_temp);
#endif

  CircularQueue->entrance += len;
  return len;
}

/**
 * [CQ_32getData 取出数据]
 * @param  CircularQueue [环形缓冲区句柄]
 * @param  targetBuf     [目标地址]
 * @param  len           [取出长度]
 * @return               [取出长度]
 */
AT_CQ_CODE_SPACE
uint32_t CQ_32getData(CQ_handleTypeDef *CircularQueue, uint32_t *targetBuf, uint32_t len)
{
  uint32_t size = 0;

  /* 此次读取的实际大小，取 可读 和 目标读取数量  两个值小的那个 */
  len = GET_MIN(len, CircularQueue->entrance - CircularQueue->exit);
  /* 原理雷同存入 */
  size = GET_MIN(len, CircularQueue->size - (CircularQueue->exit & (CircularQueue->size - 1U)));

  /* 使用DSP加速 */
#if USE_ARM_DSP_FUNC_EN
  arm_copy_q31((q31_t *)(CircularQueue->Buffer.data32Buffer + (CircularQueue->exit & (CircularQueue->size - 1U))), (q31_t *)targetBuf, size);
  arm_copy_q31((q31_t *)(CircularQueue->Buffer.data32Buffer), (q31_t *)(targetBuf + size), len - size);
#else
  uint32_t len_temp  = 4U * len;
  uint32_t size_temp = 4U * size;
  (void)memcpy(targetBuf, &CircularQueue->Buffer.data32Buffer[(CircularQueue->exit & (CircularQueue->size - 1U))], size_temp);
  (void)memcpy(targetBuf + size, CircularQueue->Buffer.data32Buffer, len_temp - size_temp);
#endif

  CircularQueue->exit += len;
  return len;
}

#if ENABLE_CQ_DEBUG_PRINTF
  /**
   * @brief 调试打印输出
   *
   * @param CircularQueue
   */
  AT_CQ_CODE_SPACE
  void CQ_DEBUG_Printf(const CQ_handleTypeDef *CircularQueue)
  {
    if(CircularQueue == NULL)
    {
      return;
    }
    printf("CircularQueue->size = %u\n", CircularQueue->size);
    printf("CircularQueue->entrance = %u\n", CircularQueue->entrance);
    printf("CircularQueue->exit = %u\n", CircularQueue->exit);
    printf("CircularQueue->Buffer.data32Buffer = %p\n", CircularQueue->Buffer.data32Buffer);
  }
#endif

#ifdef __cplusplus /**<end extern c */
}
#endif
/******************************** End of file *********************************/
