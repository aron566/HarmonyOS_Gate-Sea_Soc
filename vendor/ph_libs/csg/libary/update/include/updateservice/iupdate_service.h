/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef IUPDATE_SERVICE_H
#define IUPDATE_SERVICE_H

#include <iostream>
#include "update_helper.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iupdate_callback.h"

namespace OHOS {
namespace UpdateEngine {
class IUpdateService : public OHOS::IRemoteBroker {
public:
    enum {
        CHECK_VERSION = 1,
        DOWNLOAD,
        PAUSE_DOWNLOAD,
        RESUME_DOWNLOAD,
        UPGRADE,
        CLEAR_ERROR,
        TERMINATE_UPGRADE,
        SET_POLICY,
        GET_POLICY,
        GET_NEW_VERSION,
        GET_NEW_VERSION_DESCRIPTION,
        GET_CURRENT_VERSION,
        GET_CURRENT_VERSION_DESCRIPTION,
        GET_TASK_INFO,
        REGISTER_CALLBACK,
        UNREGISTER_CALLBACK,
        CANCEL,
        FACTORY_RESET,
        APPLY_NEW_VERSION,
        VERIFY_UPGRADE_PACKAGE
    };

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Updater.IUpdateService");
public:
    virtual int32_t RegisterUpdateCallback(const UpgradeInfo &info, const sptr<IUpdateCallback>& updateCallback) = 0;

    virtual int32_t UnregisterUpdateCallback(const UpgradeInfo &info) = 0;

    virtual int32_t CheckNewVersion(const UpgradeInfo &info) = 0;

    virtual int32_t DownloadVersion(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const DownloadOptions &downloadOptions, BusinessError &businessError) = 0;

    virtual int32_t PauseDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const PauseDownloadOptions &pauseDownloadOptions, BusinessError &businessError) = 0;

    virtual int32_t ResumeDownload(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const ResumeDownloadOptions &resumeDownloadOptions, BusinessError &businessError) = 0;

    virtual int32_t DoUpdate(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
        const UpgradeOptions &upgradeOptions, BusinessError &businessError) = 0;

    virtual int32_t ClearError(const UpgradeInfo &info, const VersionDigestInfo &versionDigest,
        const ClearOptions &clearOptions, BusinessError &businessError) = 0;

    virtual int32_t TerminateUpgrade(const UpgradeInfo &info, BusinessError &businessError) = 0;

    virtual int32_t GetNewVersion(const UpgradeInfo &info, NewVersionInfo &newVersionInfo,
        BusinessError &businessError) = 0;

    virtual int32_t GetNewVersionDescription(const UpgradeInfo &info, const VersionDigestInfo &versionDigestInfo,
        const DescriptionOptions &descriptionOptions, VersionDescriptionInfo &newVersionDescriptionInfo,
        BusinessError &businessError) = 0;

    virtual int32_t GetCurrentVersionInfo(const UpgradeInfo &info, CurrentVersionInfo &currentVersionInfo,
        BusinessError &businessError) = 0;

    virtual int32_t GetCurrentVersionDescription(const UpgradeInfo &info, const DescriptionOptions &descriptionOptions,
        VersionDescriptionInfo &currentVersionDescriptionInfo, BusinessError &businessError) = 0;

    virtual int32_t GetTaskInfo(const UpgradeInfo &info, TaskInfo &taskInfo, BusinessError &businessError) = 0;

    virtual int32_t SetUpgradePolicy(const UpgradeInfo &info, const UpgradePolicy &policy,
        BusinessError &businessError) = 0;

    virtual int32_t GetUpgradePolicy(const UpgradeInfo &info, UpgradePolicy &policy, BusinessError &businessError) = 0;

    virtual int32_t Cancel(const UpgradeInfo &info, int32_t service, BusinessError &businessError) = 0;

    virtual int32_t FactoryReset(BusinessError &businessError) = 0;

    virtual int32_t ApplyNewVersion(const UpgradeInfo &info, const std::string &miscFile,
        const std::string &packageName, BusinessError &businessError) = 0;

    virtual int32_t VerifyUpgradePackage(const std::string &packagePath, const std::string &keyPath,
        BusinessError &businessError) = 0;
};
} // namespace UpdateEngine
} // namespace OHOS
#endif // IUPDATE_SERVICE_H