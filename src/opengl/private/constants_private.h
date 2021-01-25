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

enum {
    VENDOR_ID_NVIDIA  = 0x10de,
    VENDOR_ID_INTEL   = 0x8086,
    VENDOR_ID_AMD     = 0x1002,
    VENDOR_ID_ATI     = 0x1002,
};

static const uint32_t kVendorIDs[] = {
    VENDOR_ID_NVIDIA, 
    VENDOR_ID_NVIDIA, 
    VENDOR_ID_INTEL, 
    VENDOR_ID_AMD,
    VENDOR_ID_ATI
};

static_assert(CTS_ARRAY_SIZE(kVendorNames) == CTS_ARRAY_SIZE(kVendorIDs), "Assertion failure");
static const uint32_t kNumVendors = CTS_ARRAY_SIZE(kVendorNames);

#ifdef __cplusplus
}
#endif