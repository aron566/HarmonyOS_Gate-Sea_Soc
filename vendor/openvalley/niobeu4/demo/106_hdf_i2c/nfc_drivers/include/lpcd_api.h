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

#ifndef __LPCD_API_H__
#define __LPCD_API_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

unsigned char Lpcd_Init_Register(void);
void Lpcd_IRQ_Event(void);
unsigned char Lpcd_Card_Event(void);
void Lpcd_Set_Mode(unsigned char mode);
void Lpcd_Get_ADC_Value(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __LPCD_API_H__ */