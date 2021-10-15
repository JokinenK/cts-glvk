#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <Windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <devpkey.h>
#include <stdio.h>
#include "cts/device_info.h"
#include "cts/util/uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

// Might be undefined, copied from implementation to here
static const DEVPROPKEY DeviceFirstInstallDate = { 0x83da6326, 0x97a6, 0x4088, 0x94, 0x53, 0xa1, 0x92, 0x3f, 0x57, 0x3b, 0x29, 101 };

bool ctsDeviceInfoQuery(uint32_t vendorId, uint32_t* pDeviceId, uint8_t* pUUID)
{
    CtsWin32DeviceInfo deviceInfo;
    if (ctsWin32ParseDeviceInfo(vendorId, &deviceInfo)) {
        *pDeviceId = deviceInfo.deviceId;
        memcpy(pUUID, deviceInfo.uuid, sizeof(deviceInfo.uuid));

        return true;
    }

    return false;
}

bool ctsWin32ParseDeviceInfo(uint32_t vendorId, CtsWin32DeviceInfo* pDeviceInfo) {
    bool deviceFound = false;
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(
        &GUID_DEVCLASS_DISPLAY,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_PROFILE
    );
    
    char buffer[2048];
    DWORD bufferLen;

    SP_DEVINFO_DATA deviceInfoData;
    ZeroMemory(&deviceInfoData, sizeof(deviceInfoData));
    deviceInfoData.cbSize = sizeof(deviceInfoData);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); ++i) {
        if (!SetupDiGetDeviceInstanceIdA(deviceInfoSet, &deviceInfoData, buffer, sizeof(buffer), &bufferLen)) {
            break;
        }

        uint32_t currentVendorId = 0;
        uint32_t currentDeviceId = 0;
        uint32_t currentSubSystem = 0;
        uint32_t currentRevision = 0;
        uint32_t currentDeviceSerial = 0;

        sscanf(buffer, "PCI\\VEN_%x&DEV_%x&SUBSYS_%d&REV_%x\\%*d&%x&%*d&%*d", &currentVendorId, &currentDeviceId, &currentSubSystem, &currentRevision, &currentDeviceSerial);
        if (currentVendorId == vendorId) {
            pDeviceInfo->vendorId = currentVendorId;
            pDeviceInfo->deviceId = currentDeviceId;
            strncpy(pDeviceInfo->instance, buffer, sizeof(pDeviceInfo->instance));

            uint64_t timestamp = 0;
            FILETIME timeBuffer;
            DEVPROPTYPE devPropType = DEVPROP_TYPE_FILETIME;

            if (SetupDiGetDevicePropertyW(
                deviceInfoSet,
                &deviceInfoData,
                &DeviceFirstInstallDate,
                &devPropType,
                (PBYTE)&timeBuffer,
                sizeof(timeBuffer),
                NULL,
                0 
            )) {
                LARGE_INTEGER installTime;
                installTime.HighPart = timeBuffer.dwHighDateTime;
                installTime.LowPart = timeBuffer.dwLowDateTime;
                timestamp = (installTime.QuadPart / 10000000 - 11644473600);
            }

            uint8_t serialNumber[6];
            serialNumber[0] = 0;
            serialNumber[1] = 0;
            serialNumber[2] = (uint8_t)((currentDeviceSerial & 0xFF000000) >> 24);
            serialNumber[3] = (uint8_t)((currentDeviceSerial & 0x00FF0000) >> 16);
            serialNumber[4] = (uint8_t)((currentDeviceSerial & 0x0000FF00) >> 8);
            serialNumber[5] = (uint8_t)((currentDeviceSerial & 0x000000FF) >> 0);

            CtsUUID uuid = ctsUUIDCreateV1(timestamp, 0x0000, serialNumber);
            ctsUUIDToBuffer(&uuid, pDeviceInfo->uuid);

            deviceFound = true;
            break;
        }
    }

    if (deviceInfoSet) {
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
    }

    return deviceFound;
}

#ifdef __cplusplus
}
#endif