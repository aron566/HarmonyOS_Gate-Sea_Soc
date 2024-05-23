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
/* Private includes ----------------------------------------------------------*/
#include "securec.h"
#include "los_debug.h"
#include "los_interrupt.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus /**< use C compiler */
extern "C" {
#endif
/** Private macros -----------------------------------------------------------*/
#if defined (__CC_ARM) ||  (__GNUC__)
  #pragma import(__use_no_semihosting) //确保没有从 C 库链接使用半主机的函数
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
#if 0
static void dputs(char const *s, int (*pFputc)(int n, FILE *cookie), void *cookie)
{
  unsigned int intSave;

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

int hal_trace_printf(uint32_t attr, const char *fmt, ...)
{
  char    buf[1024] = {0};
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf_s(buf, sizeof(buf), 1024 - 1, fmt, ap);
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

int printf(char const *fmt, ...)
{
  char    buf[1024] = {0};
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf_s(buf, sizeof(buf), 1024 - 1, fmt, ap);
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

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */

  return ch;
}

#ifdef __cplusplus /**< end extern c */
}
#endif
/******************************** End of file *********************************/
