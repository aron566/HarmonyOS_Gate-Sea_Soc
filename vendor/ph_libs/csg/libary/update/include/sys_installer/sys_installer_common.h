/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SYS_INSTALLER_COMMON_H
#define SYS_INSTALLER_COMMON_H

#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

namespace OHOS {
namespace SysInstaller {
constexpr const char *SYS_LOG_DIR = "/data/updater/log";
constexpr const char *SYS_LOG_FILE = "/data/updater/log/sys_installer.log";
constexpr const char *SYS_STAGE_FILE = "/data/updater/log/sys_installer_stage.log";
constexpr const char *SYS_ERROR_FILE = "/data/updater/log/sys_installer_error_code.log";

enum UpdateStatus {
    UPDATE_STATE_INIT = 0,
    UPDATE_STATE_ONGOING,
    UPDATE_STATE_FAILED,
    UPDATE_STATE_SUCCESSFUL
};
} // namespace SysInstaller
} // namespace OHOS
#endif // SYS_INSTALLER_COMMON_H
