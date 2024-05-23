/*
 * Copyright (c) 2022 Hihope Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "main.h"
#include <stdarg.h>
#include "securec.h"
#include "los_debug.h"
#include "los_interrupt.h"

static void dputs(char const *s, int (*pFputc)(int n, FILE *cookie), void *cookie)
{
    unsigned int intSave;

    intSave = LOS_IntLock();
    while (*s) {
        pFputc(*s++, cookie);
    }
    LOS_IntRestore(intSave);
}

int hal_trace_printf(uint32_t attr, const char *fmt, ...)
{
    char buf[1024] = { 0 };
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf_s(buf, sizeof(buf), 1024 - 1, fmt, ap);
    if (len > 0) {
         dputs(buf, fputc, 0);
    } else {
        dputs("printf error!\n", fputc, 0);
    }
    va_end(ap);
    return len;
}

int printf(char const  *fmt, ...)
{
    char buf[1024] = { 0 };
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf_s(buf, sizeof(buf), 1024 - 1, fmt, ap);
    if (len > 0) {
         dputs(buf, fputc, 0);
    } else {
         dputs("printf error!\n", fputc, 0);
    }
    va_end(ap);
    return len;
}
