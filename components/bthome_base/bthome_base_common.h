/*
 BTHome base common

 Author: Attila Farago
 */

#pragma once

#include <string>

namespace bthome_base
{
    typedef uint64_t mac_address_t;
    typedef uint8_t bthome_measurement_t;
    typedef struct
    {
      bthome_measurement_t id;
      float value;
    } bthome_measurement_record_t;

    std::string addr64_to_str(mac_address_t address);
    std::string addr_to_str(const uint8_t *address);
    mac_address_t addr_to_uint64(const uint8_t *address);

    typedef enum
    {
        HaBleType_uint = 0,
        HaBleType_sint = 1,
        HaBleType_float = 2,
        HaBleType_string = 3,
        HaBleType_MAC = 4
    } HaBleTypes_e;

    typedef struct
    {
        uint8_t factor_raw;
        uint16_t factor_multiple;
        uint8_t len_in_bytes;
        HaBleTypes_e data_format;
    } BTHomeDataFormat;

    BTHomeDataFormat getDataFormat(bthome_measurement_t obj_meas_type);

    // Sign extend trick
    // http://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
    template <typename T, unsigned B>
    inline T signextend(const T x)
    {
        struct
        {
            T x : B;
        } s;
        return s.x = x;
    }
}