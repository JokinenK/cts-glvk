#include <stdio.h>
#include <string.h>
#include <cts/uuid.h>

#ifdef __cplusplus
extern "C" {
#endif

static const uint64_t kUUIDv1TimeOffset = 122192928000000000;

static unsigned char parseNibble(char chr);
static void setTimeAndVersion(CtsUUID* pUUID, uint64_t timestamp, uint8_t version);

CtsUUID ctsUUIDFromString(const char* pSource, size_t sourceLen) {
    CtsUUID uuid = {};

    if (sourceLen < 32 ) {
        return uuid;
    }

    bool haveHyphens = false;
    if (pSource[8] == '-' && pSource[13] == '-' && pSource[18] == '-' && pSource[23] == '-') {
        if (sourceLen < 36 ) {
            return uuid;
        }

        haveHyphens = true;
    }

    const char* it = pSource;
    for (int i = 0; i < 8; ++i) {
        uuid.timeLow = (uuid.timeLow << 4) | parseNibble(*it++);
    }

    if (haveHyphens) { ++it; }
    for (int i = 0; i < 4; ++i) {
        uuid.timeMid = (uuid.timeMid << 4) | parseNibble(*it++);
    }

    if (haveHyphens) { ++it; }
    for (int i = 0; i < 4; ++i) {
        uuid.timeHiAndVersion = (uuid.timeHiAndVersion << 4) | parseNibble(*it++);
    }

    if (haveHyphens) { ++it; }
    for (int i = 0; i < 4; ++i) {
        uuid.clockSeq = (uuid.clockSeq << 4) | parseNibble(*it++);
    }

    if (haveHyphens) { ++it; }
    for (int i = 0; i < 6; ++i) {
        uuid.node[i] = (parseNibble(*it++) << 4) | parseNibble(*it++);
    }

    return uuid;
}

CtsUUID ctsUUIDCreateV1(uint64_t timestamp, uint16_t clockSeq, const uint8_t node[6]) {
    CtsUUID uuid;
    setTimeAndVersion(&uuid, timestamp, CTS_UUID_TIME_BASED);
    uuid.clockSeq = (clockSeq & 0x3FFF) | 0x8000;
    memcpy(uuid.node, node, sizeof(uuid.node));

    return uuid;
}

CtsUUIDVersion ctsUUIDVersion(const CtsUUID* pUUID) {
    return (CtsUUIDVersion)(pUUID->timeHiAndVersion >> 12);
}

uint64_t ctsUUIDTimestamp(const CtsUUID* pUUID) {
    uint64_t timestamp = (
        ((uint64_t)pUUID->timeHiAndVersion & 0x00000FFF) << 48 |
        ((uint64_t)pUUID->timeMid          & 0x0000FFFF) << 32 |
        ((uint64_t)pUUID->timeLow          & 0xFFFFFFFF) << 0
    );

    return (timestamp - kUUIDv1TimeOffset) / 10000000;
}

void ctsUUIDToBuffer(const CtsUUID* pUUID, uint8_t buffer[16]) {
    buffer[0] = (uint8_t)(pUUID->timeLow >> 24) & 0xFF;
    buffer[1] = (uint8_t)(pUUID->timeLow >> 16) & 0xFF;
    buffer[2] = (uint8_t)(pUUID->timeLow >> 8) & 0xFF;
    buffer[3] = (uint8_t)(pUUID->timeLow >> 0) & 0xFF;

    buffer[4] = (uint8_t)(pUUID->timeMid >> 8) & 0xFF;
    buffer[5] = (uint8_t)(pUUID->timeMid >> 0) & 0xFF;

    buffer[6] = (uint8_t)(pUUID->timeHiAndVersion >> 8) & 0xFF;
    buffer[7] = (uint8_t)(pUUID->timeHiAndVersion >> 0) & 0xFF;

    buffer[8] = (uint8_t)(pUUID->clockSeq >> 8) & 0xFF;
    buffer[9] = (uint8_t)(pUUID->clockSeq >> 0) & 0xFF;

    buffer[10] = (uint8_t)(pUUID->node[0]);
    buffer[11] = (uint8_t)(pUUID->node[1]);
    buffer[12] = (uint8_t)(pUUID->node[2]);
    buffer[13] = (uint8_t)(pUUID->node[3]);
    buffer[14] = (uint8_t)(pUUID->node[4]);
    buffer[15] = (uint8_t)(pUUID->node[5]);
}

size_t ctsUUIDToString(const CtsUUID* pUUID, unsigned char* pBuffer, size_t bufferLen, bool useHyphens) {
    size_t offset = 0;

    offset += snprintf(pBuffer + offset, bufferLen - offset, "%08x", pUUID->timeLow);
    if (useHyphens) { offset += snprintf(pBuffer + offset, bufferLen - offset, "-"); }
    
    offset += snprintf(pBuffer + offset, bufferLen - offset, "%04x", pUUID->timeMid);
    if (useHyphens) { offset += snprintf(pBuffer + offset, bufferLen - offset, "-"); }
    
    offset += snprintf(pBuffer + offset, bufferLen - offset, "%04x", pUUID->timeHiAndVersion);
    if (useHyphens) { offset += snprintf(pBuffer + offset, bufferLen - offset, "-"); }
    
    offset += snprintf(pBuffer + offset, bufferLen - offset, "%04x", pUUID->clockSeq);
    if (useHyphens) { offset += snprintf(pBuffer + offset, bufferLen - offset, "-"); }
    
    for (uint32_t i = 0; i < sizeof(pUUID->node); ++i) {
        offset += snprintf(pBuffer + offset, bufferLen - offset, "%02x", pUUID->node[i]);
    }

    return offset;
}

static unsigned char parseNibble(char chr) {
    if (chr >= 'a' && chr <= 'f') {
        return (chr - 'a' + 10);
    } else if (chr >= 'A' && chr <= 'F') {
        return (chr - 'A' + 10);
    } else if (chr >= '0' && chr <= '9') {
        return (chr - '0');
    }

    return 0;
}

static void setTimeAndVersion(CtsUUID* pUUID, uint64_t timestamp, uint8_t version) {
    timestamp *= 10000000;
    timestamp += kUUIDv1TimeOffset;

    pUUID->timeHiAndVersion = (uint16_t)((timestamp >> 48) & 0x0FFF) + (version << 12);
    pUUID->timeMid          = (uint16_t)((timestamp >> 32) & 0xFFFF);
    pUUID->timeLow          = (uint32_t)((timestamp >>  0) & 0xFFFFFFFF);
}

#ifdef __cplusplus
}
#endif