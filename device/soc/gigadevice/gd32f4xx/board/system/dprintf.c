/*
 * Copyright (c) 2021 GOODIX.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdarg.h>
#include <stdio.h>
#include "securec.h"

#define BUFSIZE  256

static void dputs(char const *s, int (*pFputc)(int n, FILE *cookie), void *cookie)
{
    while (*s) {
        pFputc(*s++, cookie);
    }
}

int printf(const char *__restrict __format, ...)
{
    char buf[BUFSIZE] = { 0 };
    int len;
    va_list ap;
    va_start(ap, __format);
    len = vsnprintf_s(buf, sizeof(buf), BUFSIZE - 1, __format, ap);
    va_end(ap);
    if(len > 0) {
        dputs(buf, fputc, 0);
    } else {
        dputs("printf error!\n", fputc, 0);
    }

    return len;
}

int sprintf(char *__restrict   __s, const char *__restrict  __format, ...)
{
    va_list args;
    int val;

    va_start(args, __format);
    val = vsprintf_s(__s,  BUFSIZE - 1,  __format,  args);
    va_end(args);

    return val;
}

int fsync(int fd)
{
    (void *)fd;
    return 0;
}