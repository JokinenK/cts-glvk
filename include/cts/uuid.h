#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsUUID {
    uint32_t timeLow;
    uint16_t timeMid;
    uint16_t timeHiAndVersion;
    uint16_t clockSeq;
    uint8_t node[6];
} CtsUUID;

typedef enum CtsUUIDVersion {
    CTS_UUID_TIME_BASED = 0x01,
    CTS_UUID_DCE_UID    = 0x02,
    CTS_UUID_NAME_BASED = 0x03,
    CTS_UUID_RANDOM     = 0x04
} CtsUUIDVersion;

CtsUUID ctsUUIDFromString(const char* pSource, size_t sourceLen);
CtsUUID ctsUUIDCreateV1(uint64_t unixtime, uint16_t clockSeq, const uint8_t node[6]);

CtsUUIDVersion ctsUUIDVersion(const CtsUUID* pUUID);
uint64_t ctsUUIDUnixTime(const CtsUUID* pUUID);

void ctsUUIDToBuffer(const CtsUUID* pUUID, uint8_t buffer[16]);
size_t ctsUUIDToString(const CtsUUID* pUUID, unsigned char* pBuffer, size_t bufferLen, bool haveHyphens);

#ifdef __cplusplus
}
#endif