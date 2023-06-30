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

    uint8_t getByteNumber(uint8_t obj_meas_type);
    uint16_t getFactor(uint8_t obj_meas_type);      // 1,10,100,1000
    uint16_t getFactorLog10(uint8_t obj_meas_type); // 0,1,2,3
    uint8_t getDataFormat(uint8_t obj_meas_type);

    static const float MEAS_TYPES_FACTORS[] = {
        1,     // 0
        0.1,   // 1
        0.01,  // 2
        0.001, // 3
        0.0001 // 4 - not possible
    };
    static const uint16_t MEAS_TYPES_FACTORS_POSITIVE[] = {
        1,    // 0
        10,   // 1
        100,  // 2
        1000, // 3
        10000 // 4 - not possible
    };

    typedef enum
    {
        HaBleType_uint = 0,
        HaBleType_sint = 1,
        HaBleType_float = 2,
        HaBleType_string = 3,
        HaBleType_MAC = 4
    } HaBleTypes_e;
}