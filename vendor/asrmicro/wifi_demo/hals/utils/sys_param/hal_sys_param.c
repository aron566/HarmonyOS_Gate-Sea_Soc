/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

const char *HalGetSerial(void)
{
    unsigned char macAddr[6] = {0x00};
    static char serialNumber[13] = {0x00};
    int ret;
    extern int lega_wlan_get_mac_address(unsigned char *addr);
    lega_wlan_get_mac_address(macAddr);
    ret = snprintf(serialNumber, 12, "%02X%02X%02X%02X%02X%02X",
                   macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    if (ret < 0) {
        printf("snprintf ret fail!\n");
    }
    return serialNumber;
}