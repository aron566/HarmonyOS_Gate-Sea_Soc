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

#include "hal_sys_param.h"
#include "stdint.h"

#define HEX_A               0xa
#define CHAR_NUM_OFFSET     0x30
#define CHAR_CAPITAL_OFFSET 0x37
#define SERIAL_LEN          8
#define UID_LEN             16
#define UID_SHIFT           4
#define UID_HIGH_MASK       0xf0
#define UID_LOW_MASK        0x0f
#define ZERO      0
#define ONE       1
#define TWO       2
#define THREE     3
#define FOUR      4
#define FIVE      5
#define SIX       6
#define SEVEN     7
#define EIGHT     8
#define NINE      9
#define TEN       10
#define ELEVEN    11
#define TWELVE    12
#define THIRTEEN  13
#define FOURTEEN  14
#define FIFTEEN   15

static char serialNumber[(SERIAL_LEN >> 1) + 1];

static char Hex2Char(uint8_t hex)
{
    if (hex < HEX_A) {
        return hex + CHAR_NUM_OFFSET;
    } else {
        return hex + CHAR_CAPITAL_OFFSET;
    }
}

const char* HalGetSerial(void)
{
    int i;
    int j = 0;
    uint8_t UidTmp[SERIAL_LEN];
    uint8_t Uid[UID_LEN];
    
    if (sys_device_uid_get(Uid) == 0) {
        UidTmp[ZERO] = Uid[THREE];
        UidTmp[ONE] = Uid[FOUR];
        UidTmp[TWO] = Uid[FIVE];
        UidTmp[THREE] = Uid[SIX];
        UidTmp[FOUR] = Uid[TEN];
        UidTmp[FIVE] = Uid[THIRTEEN];
        UidTmp[SIX] = Uid[FOURTEEN];
        UidTmp[SEVEN] = Uid[FIFTEEN];
    }

    for (i = 0; i < SERIAL_LEN; i++) {
        uint8_t highFour, lowFour;
        highFour = (UidTmp[i] & UID_HIGH_MASK) >> UID_SHIFT;
        serialNumber[j] = Hex2Char(highFour);
        j++;
        lowFour = UidTmp[i] & UID_LOW_MASK;
        serialNumber[j] = Hex2Char(lowFour);
        j++;
    }

    return serialNumber;
}