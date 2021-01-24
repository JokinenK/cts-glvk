#pragma once

#include <assert.h>
#include <stdint.h>
#include <cts/macros.h>

#ifdef __cplusplus
extern "C" {
#endif

static const char kNVidiaVendorName[]   = "nvidia";
static const char kNouveauVendorName[]  = "nouveau";
static const char kIntelVendorName[]    = "intel";
static const char kAMDVendorName[]      = "amd";
static const char kATIVendorName[]      = "ati";
static const char* kVendorNames[]       = {
    kNVidiaVendorName,
    kNouveauVendorName,
    kIntelVendorName,
    kAMDVendorName,
    kATIVendorName
};

static const uint32_t kNVidiaVendorID  = 0x10de;
static const uint32_t kIntelVendorID   = 0x8086;
static const uint32_t kAMDVendorID     = 0x1002;
static const uint32_t kATIVendorID     = 0x1002;
static const uint32_t kVendorIDs[]     = {
    kNVidiaVendorID, 
    kNVidiaVendorID, 
    kIntelVendorID, 
    kAMDVendorID,
    kATIVendorID
};

static_assert(CTS_ARRAY_SIZE(kVendorNames) == CTS_ARRAY_SIZE(kVendorIDs), "Assertion failure");
static const uint32_t kNumVendors = CTS_ARRAY_SIZE(kVendorNames);

#ifdef __cplusplus
}
#endif