/**
 *  @file circular_queue.h
 *
 *  @date 2020/6/25
 *
 *  @author aron566
 *
 *  @brief 缓冲区大小需为2的n次方(最高位为1其余为0)
 *         v1.4 2022.10.28 优化部分代码及注释.
 *
 *  @version v1.4
 */
#ifndef CIRCULARQUEUE_H_
#define CIRCULARQUEUE_H_
#ifdef __cplusplus /**<use C compiler */
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdlib.h>
#include <string.h>
/** Private includes ---------------------------------------------------------*/
/** Private defines ----------------------------------------------------------*/
#define ENABLE_CQ_DEBUG_PRINTF  0 /**< 调试打印输出 */
/** Exported typedefines -----------------------------------------------------*/
/** 数据结构体 */
typedef struct
{
  union {
    uint8_t   *data8Buffer;		/**< for 8bit buffer*/
    uint16_t  *data16Buffer;	/**< for 16bit buffer*/
    uint32_t  *data32Buffer;	/**< for 32bit buffer*/
  }Buffer;
  uint32_t size;
  uint32_t entrance;
  uint32_t exit;
  bool is_malloc;
}CQ_handleTypeDef;

/* 缓冲区大小 */
typedef enum
{
  CQ_BUF_128B = 128U,
  CQ_BUF_256B = 256U,
  CQ_BUF_512B = 512U,
  CQ_BUF_1KB  = 1U * 1024U,
  CQ_BUF_2KB  = 2U * 1024U,
  CQ_BUF_4KB  = 4U * 1024U,
  CQ_BUF_8KB  = 8U * 1024U,
  CQ_BUF_1MB  = 1024U * 1024U,
}CQ_BUF_SIZE_ENUM_TypeDef;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/

/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/**
 * @brief 环形队列是否为空
 *
 * @param CircularQueue 环形队列句柄
 * @return true 为空
 * @return false 不为空
 */
bool CQ_isEmpty(CQ_handleTypeDef *CircularQueue);

/**
 * @brief 环形队列是否满
 *
 * @param CircularQueue 环形队列句柄
 * @return true 为满
 * @return false 不为满
 */
bool CQ_isFull(CQ_handleTypeDef *CircularQueue);

/**
 * @brief 清空环形队列数据长度记录
 *
 * @param CircularQueue 环形队列句柄
 */
void CQ_emptyData(CQ_handleTypeDef *CircularQueue);

/**
 * @brief 获取环形队列可用数据长度
 *
 * @param CircularQueue 环形队列句柄
 * @return uint32_t 数据长度
 */
uint32_t CQ_getLength(CQ_handleTypeDef *CircularQueue);

/**
 * @brief 手动缩减缓冲区长度--用作：错误帧偏移-正确帧读取后剔除
 *
 * @param CircularQueue 环形队列句柄
 * @param len 剔除长度
 */
void CQ_ManualOffsetInc(CQ_handleTypeDef *CircularQueue, uint32_t len);

/**
 * @brief 跳转到指定有效数据，用于快速丢弃无效头部数据
 *
 * @param cb 环形队列句柄
 * @param Data 目标数据
 * @return uint32_t 剩余可读长度
 */
uint32_t CQ_skipInvaildU8Header(CQ_handleTypeDef *CircularQueue, uint8_t Data);

/**
 * @brief 跳转到指定16位有效数据，用于快速丢弃无效头部数据
 *
 * @param cb 环形队列句柄
 * @param Data 高位在前-低位在后存储的16位数据
 * @return uint32_t 剩余可读长度
 */
uint32_t CQ_skipInvaildModbusU16Header(CQ_handleTypeDef *CircularQueue, uint16_t Data);

/**
 * @brief 跳转到指定32位有效数据，用于快速丢弃无效头部数据
 *
 * @param cb 环形队列句柄
 * @param Data 高位在前-低位在后存储的32位数据
 * @return uint32_t 剩余可读长度
 */
uint32_t CQ_skipInvaildModbusU32Header(CQ_handleTypeDef *CircularQueue, uint32_t Data);

/**
 * @brief 跳转到指定16位有效数据，用于快速丢弃无效头部数据
 *
 * @param cb 环形队列句柄
 * @param Data 低位在前-高位在后存储的16位数据
 * @return uint32_t 剩余可读长度
 */
uint32_t CQ_skipInvaildU16Header(CQ_handleTypeDef *CircularQueue, uint16_t Data);

/**
 * @brief 跳转到指定32位有效数据，用于快速丢弃无效头部数据
 *
 * @param cb 环形队列句柄
 * @param Data 低位在前-高位在后存储的32位数据
 * @return uint32_t 剩余可读长度
 */
uint32_t CQ_skipInvaildU32Header(CQ_handleTypeDef *CircularQueue, uint32_t Data);

/**
 * @brief 删除动态分配的环形缓冲队列
 *
 * @param CircularQueue 环形队列句柄
 */
void CQ_Delete(CQ_handleTypeDef *CircularQueue);

/**
 * @brief 8位环形队列初始化
 *
 * @param CircularQueue 环形队列句柄
 * @param memAdd 需要管理的内存
 * @param len 缓冲区长度
 * @return true 成功
 * @return false 失败
 */
bool CQ_init(CQ_handleTypeDef *CircularQueue, uint8_t *memAdd, uint32_t len);

/**
 * @brief 动态创建8位环形队列
 *
 * @param buffSize 环形队列长度
 * @return CQ_handleTypeDef* 环形队列句柄
 */
CQ_handleTypeDef *CQ_Create(uint32_t buffSize);

/**
 * @brief 取出N个8位数据到缓冲区
 *
 * @param CircularQueue 环形队列句柄
 * @param targetBuf 存储区
 * @param len 数据长度，N个8位
 * @return uint32_t 获取到的数据长度
 */
uint32_t CQ_getData(CQ_handleTypeDef *CircularQueue, uint8_t *targetBuf, uint32_t len);

/**
 * @brief 加入N个8位数据到队列
 *
 * @param CircularQueue 环形队列句柄
 * @param sourceBuf 数据区域
 * @param len 数据长度
 * @return uint32_t 加入数据长度
 */
uint32_t CQ_putData(CQ_handleTypeDef *CircularQueue, const uint8_t *sourceBuf, uint32_t len);

/**
 * @brief 临时取出环形队列数据，为了分步取出完整数据
 *
 * @param CircularQueue 环形队列句柄
 * @param targetBuf 存储区
 * @param len 数据长度，N个8位
 * @return uint32_t 获取到的数据长度
 */
uint32_t CQ_ManualGetData(CQ_handleTypeDef *CircularQueue, uint8_t *targetBuf, uint32_t len);

/**
 * @brief 临时取出指定偏移下的数据，目的为了验证数据，判断帧头等
 *
 * @param CircularQueue 环形队列句柄
 * @param index 偏移
 * @return uint8_t 取出的数据
 */
uint8_t CQ_ManualGet_Offset_Data(CQ_handleTypeDef *CircularQueue, uint32_t index);

/**
 * @brief 16位环形队列初始化
 *
 * @param CircularQueue 环形队列句柄
 * @param memAdd 需要管理的内存
 * @param len 缓冲区长度
 * @return true 成功
 * @return false 失败
 */
bool CQ_16_init(CQ_handleTypeDef *CircularQueue, uint16_t *memAdd, uint32_t len);

/**
 * @brief 动态创建16位环形队列
 *
 * @param buffSize 环形队列长度
 * @return CQ_handleTypeDef* 环形队列句柄
 */
CQ_handleTypeDef *CQ_16Create(uint32_t buffSize);

/**
 * @brief 加入N个16位数据到队列
 *
 * @param CircularQueue 环形队列句柄
 * @param sourceBuf 数据区域
 * @param len 数据长度
 * @return uint32_t 加入数据长度
 */
uint32_t CQ_16putData(CQ_handleTypeDef *CircularQueue, const uint16_t *sourceBuf, uint32_t len);

/**
 * @brief 取出N个16位数据到缓冲区
 *
 * @param CircularQueue 环形队列句柄
 * @param targetBuf 存储区
 * @param len 数据长度，N个16位
 * @return uint32_t 获取到的数据长度
 */
uint32_t CQ_16getData(CQ_handleTypeDef *CircularQueue, uint16_t *targetBuf, uint32_t len);

/**
 * @brief 32位环形队列初始化
 *
 * @param CircularQueue 环形队列句柄
 * @param memAdd 需要管理的内存
 * @param len 缓冲区长度
 * @return true 成功
 * @return false 失败
 */
bool CQ_32_init(CQ_handleTypeDef *CircularQueue, uint32_t *memAdd, uint32_t len);

/**
 * @brief 动态创建32位环形队列
 *
 * @param buffSize 环形队列长度
 * @return CQ_handleTypeDef* 环形队列句柄
 */
CQ_handleTypeDef *CQ_32Create(uint32_t buffSize);

/**
 * @brief 加入N个32位数据到队列
 *
 * @param CircularQueue 环形队列句柄
 * @param sourceBuf 数据区域
 * @param len 数据长度
 * @return uint32_t 加入数据长度
 */
uint32_t CQ_32putData(CQ_handleTypeDef *CircularQueue, const uint32_t *sourceBuf, uint32_t len);

/**
 * @brief 取出N个32位数据到缓冲区
 *
 * @param CircularQueue 环形队列句柄
 * @param targetBuf 存储区
 * @param len 数据长度，N个32位
 * @return uint32_t 获取到的数据长度
 */
uint32_t CQ_32getData(CQ_handleTypeDef *CircularQueue, uint32_t *targetBuf, uint32_t len);

#if ENABLE_CQ_DEBUG_PRINTF
  /**
   * @brief 调试打印输出
   *
   * @param CircularQueue
   */
  void CQ_DEBUG_Printf(const CQ_handleTypeDef *CircularQueue);
#endif

#ifdef __cplusplus /**<end extern c */
}
#endif
#endif
/******************************** End of file *********************************/
