/*
 BTHome base common

 Author: Attila Farago
 */

#include <string>
#include <pgmspace.h>

#include "bthome_base_common.h"
#include "bthome_common_generated.h"

namespace bthome_base
{
    std::string addr64_to_str(uint64_t address)
    {
        char buffer[18];
        snprintf(buffer, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x",
                 (uint8_t)(address >> 40) & 0xff, (uint8_t)(address >> 32) & 0xff, (uint8_t)(address >> 24) & 0xff,
                 (uint8_t)(address >> 16) & 0xff, (uint8_t)(address >> 8) & 0xff, (uint8_t)(address >> 0) & 0xff);
        return buffer;
    }

    uint64_t addr_to_uint64(const uint8_t *address)
    {
        // uint64_t u = uint64_t(address[0] & 0xFF) << 40 | uint64_t(address[1] & 0xFF) << 32 | uint64_t(address[2] & 0xFF) << 24 | uint64_t(address[3] & 0xFF) << 16 | uint64_t(address[4] & 0xFF) << 8 | uint64_t(address[5] & 0xFF) << 0;
        uint64_t u = uint64_t(address[0]) << 40 | uint64_t(address[1]) << 32 | uint64_t(address[2]) << 24 | uint64_t(address[3]) << 16 | uint64_t(address[4]) << 8 | uint64_t(address[5]) << 0;
        return u;
    }

    std::string addr_to_str(const uint8_t *address)
    {
        char buffer[18];
        snprintf(buffer, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x",
                 (uint8_t)address[0], (uint8_t)address[1], (uint8_t)address[2],
                 (uint8_t)address[3], (uint8_t)address[4], (uint8_t)address[5]);
        return buffer;
    };

    static const uint16_t MEAS_TYPES_FACTORS_POSITIVE[] = {
        1,    // 0
        10,   // 1
        100,  // 2
        1000, // 3
        10000 // 4 - not possible
    };

    BTHomeDataFormat getDataFormat(uint8_t obj_meas_type)
    {
        // check range
        if (obj_meas_type >= sizeof(MEAS_TYPES_FLAGS) / sizeof(uint8_t))
            return {};

        const uint8_t meas_type_flags = pgm_read_byte_near(MEAS_TYPES_FLAGS + obj_meas_type);
        uint8_t factor_raw = (meas_type_flags & 0b01100000) >> 5;
        BTHomeDataFormat retval = {
            .factor_raw = factor_raw,
            .factor_multiple = MEAS_TYPES_FACTORS_POSITIVE[factor_raw],
            .len_in_bytes = (uint8_t)(meas_type_flags & 0b00000111),
            .data_format = (HaBleTypes_e)((meas_type_flags & 0b00011000) >> 3)};
        return retval;
    }
}
