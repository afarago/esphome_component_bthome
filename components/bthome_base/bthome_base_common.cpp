/*
 BTHome base common

 Author: Attila Farago
 */

#include <string>

#include "bthome_base_common.h"

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
}
