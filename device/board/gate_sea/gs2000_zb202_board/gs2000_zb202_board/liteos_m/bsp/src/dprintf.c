/**
 *  @file dprintf.c
 *  @brief printf实现.
 *  @author aron566 (wei.chen@gate-sea.com)
 *  @version v0.0.1 aron566 2024.05.22 16:33 初始版本.
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
#include <stdarg.h>
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
#include "securec.h"
#include "los_debug.h"
#include "los_interrupt.h"
#include "hiview_def.h"
#include "hiview_output_log.h"
// #include "hiview_log.h"
#include "uart.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/
#if defined(__CC_ARM) || (__GNUC__)
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
  #ifdef USE_LINK_SMALL_PRINTF
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
    #define GETCHAR_PROTOTYPE int __io_getchar(void)
  #else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
    #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
  #endif
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
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
#if 1
static bool HilogProc_Impl(const HiLogContent *hilogContent, uint32_t len)
{
  char tempOutStr[LOG_FMT_MAX_LEN];
  tempOutStr[0] = 0,tempOutStr[1] = 0;
  if (LogContentFmt(tempOutStr, sizeof(tempOutStr), hilogContent) > 0)
  {
    printf(tempOutStr);
  }
  return true;
}

/**
 * @brief 输出字符串
 *
 * @param s 字符串
 * @param pFputc 输出函数
 * @param cookie
 */
static void dputs(char const *s, int (*pFputc)(int n, FILE *cookie), void *cookie)
{
  uint32_t intSave;

  intSave = LOS_IntLock();
  while (*s)
  {
    pFputc(*s++, cookie);
  }
  LOS_IntRestore(intSave);
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
 *
 *       Public code
 *
 ********************************************************************************
 */
static char Str_Buffer[256U];
int hal_trace_printf(uint32_t attr, const char *fmt, ...)
{
  // if (attr > PRINT_LEVEL)
  // {
  //   return;
  // }
  // char    buf[1024U] = {0};
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf_s(Str_Buffer, sizeof(Str_Buffer), sizeof(Str_Buffer) - 1U, fmt, ap);
  if (len > 0)
  {
    dputs(Str_Buffer, fputc, 0);
  }
  else
  {
    dputs("hal printf error!\r\n", fputc, 0);
    // Str_Buffer[0] = '[';
    // dputs(Str_Buffer, fputc, 0);
  }
  va_end(ap);
  return len;
}

int printf(char const *fmt, ...)
{
  char    buf[256U] = {0};
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1U, fmt, ap);
  if (len > 0)
  {
    dputs(buf, fputc, 0);
  }
  else
  {
    dputs("printf error!\n", fputc, 0);
  }
  va_end(ap);
  return len;
}
#endif

/**
 * @brief 初始化DFX子系统输出接口
 *
 */
void Hilog_Init(void)
{
  /* 注册日志输出接口 */
  HiviewRegisterHilogProc(HilogProc_Impl);
}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  USART_SendData(UART1, (uint8_t)ch);
  while(!USART_GetStatus(UART1, USART_TRANSMITTER_EMPTY_STATUS));
  return ch;
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/
