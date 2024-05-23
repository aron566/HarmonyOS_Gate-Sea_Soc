/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
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

#include <stdio.h>
#include "ohos_run.h"

/* HelloWorld例子 */
static void Niobeu4HelloWorld(void)
{
    printf("**********************\n");
    printf("*  [Openvalley/Niobeu4] *\n");
    printf("*    Hello world.    *\n");
    printf("**********************\n");
    return;
}

/* 声明应用程序函数入口 */
OHOS_APP_RUN(Niobeu4HelloWorld);
