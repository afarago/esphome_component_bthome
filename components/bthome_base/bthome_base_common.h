/*
 BTHome base common

 Author: Attila Farago
 */

#pragma once

#include <string>

namespace bthome_base
{
    std::string addr64_to_str(uint64_t address);
    std::string addr_to_str(const uint8_t *address);
    uint64_t addr_to_uint64(const uint8_t *address);
}
