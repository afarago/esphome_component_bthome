#include <vector>
#include <map>
#include <pgmspace.h>

#include "esphome/core/log.h"
#include "bthome_parser.h"

namespace esphome {

static const char *const TAG = "bthome_parser";

typedef enum
{
  HaBleType_uint = 0,
  HaBleType_sint = 1,
  // HaBleType_float = 2,
  // HaBleType_string = 3,
  // HaBleType_MAC = 4
} HaBleTypes_e;

// Sign extend trick
// http://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
template<typename T, unsigned B> inline T signextend(const T x) {
  struct {
    T x : B;
  } s;
  return s.x = x;
}

uint16_t combine_bytes_little_endian_u16(const uint8_t *data) { return ((data[1] & 0xFF) << 8) | (data[0] & 0xFF); }
uint32_t combine_bytes_little_endian_u32(const uint8_t *data) { return ((data[3] & 0xFF) << 24) | ((data[2] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[0] & 0xFF); }

static const float MEAS_TYPES_FACTORS[] = {
  1,      // 0
  0.1,    // 1
  0.01,   // 2
  0.001,  // 3
  0.0001  // 4 - not possible
};

static const uint8_t PROGMEM MEAS_TYPES_FLAGS[] = { // DataLen 123 bits, DataType 45 bits, Factor 67 bits
  0x01,	 // 0x00 | packet id | uint8 (1 byte) | datatype: 0 | factor_exp10: 1 | example: 9
  0x01,	 // 0x01 | battery | uint8 (1 byte) | datatype: 0 | factor_exp10: 1 | example: 0161
  0x4A,	 // 0x02 | temperature | sint16 (2 bytes) | datatype: 1 | factor_exp10: 0.01 | example: 02CA09
  0x42,	 // 0x03 | humidity | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.01 | example: 03BF13
  0x43,	 // 0x04 | pressure | uint24 (3 bytes) | datatype: 0 | factor_exp10: 0.01 | example: 04138A01
  0x43,	 // 0x05 | illuminance | uint24 (3 bytes) | datatype: 0 | factor_exp10: 0.01 | example: 05138A14
  0x42,	 // 0x06 | mass (kg) | uint16 (2 byte) | datatype: 0 | factor_exp10: 0.01 | example: 065E1F
  0x42,	 // 0x07 | mass (lb) | uint16 (2 byte) | datatype: 0 | factor_exp10: 0.01 | example: 073E1D
  0x4A,	 // 0x08 | dewpoint | sint16 (2 bytes) | datatype: 1 | factor_exp10: 0.01 | example: 08CA06
  0x01,	 // 0x09 | count | uint (1 bytes) | datatype: 0 | factor_exp10: 1 | example: 0960
  0x63,	 // 0x0A | energy | uint24 (3 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 0A138A14
  0x43,	 // 0x0B | power | uint24 (3 bytes) | datatype: 0 | factor_exp10: 0.01 | example: 0B021B00
  0x62,	 // 0x0C | voltage | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 0C020C
  0x02,	 // 0x0D | pm2.5 | uint16 (2 bytes) | datatype: 0 | factor_exp10: 1 | example: 0D120C
  0x02,	 // 0x0E | pm10 | uint16 (2 bytes) | datatype: 0 | factor_exp10: 1 | example: 0E021C
  0x01,	 // 0x0F | generic boolean | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 0F01
  0x01,	 // 0x10 | power | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1001
  0x01,	 // 0x11 | opening | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1100
  0x02,	 // 0x12 | co2 | uint16 (2 bytes) | datatype: 0 | factor_exp10: 1 | example: 12E204
  0x02,	 // 0x13 | tvoc | uint16 (2 bytes) | datatype: 0 | factor_exp10: 1 | example: 133301
  0x42,	 // 0x14 | moisture | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.01 | example: 14020C
  0x01,	 // 0x15 | battery | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1501
  0x01,	 // 0x16 | battery charging | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1601
  0x01,	 // 0x17 | carbon monoxide | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1700
  0x01,	 // 0x18 | cold | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1801
  0x01,	 // 0x19 | connectivity | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1900
  0x01,	 // 0x1A | door | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1A00
  0x01,	 // 0x1B | garage door | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1B01
  0x01,	 // 0x1C | gas | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1C01
  0x01,	 // 0x1D | heat | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1D00
  0x01,	 // 0x1E | light | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 10
  0x01,	 // 0x1F | lock | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 1F01
  0x01,	 // 0x20 | moisture | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2001
  0x01,	 // 0x21 | motion | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2100
  0x01,	 // 0x22 | moving | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2201
  0x01,	 // 0x23 | occupancy | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2301
  0x01,	 // 0x24 | plug | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2400
  0x01,	 // 0x25 | presence | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2500
  0x01,	 // 0x26 | problem | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2601
  0x01,	 // 0x27 | running | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2701
  0x01,	 // 0x28 | safety | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2800
  0x01,	 // 0x29 | smoke | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2901
  0x01,	 // 0x2A | sound | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2A00
  0x01,	 // 0x2B | tamper | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2B00
  0x01,	 // 0x2C | vibration | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2C01
  0x01,	 // 0x2D | window | uint8 (1 byte) | datatype: 0 | factor_exp10:  | example: 2D01
  0x01,	 // 0x2E | humidity | uint8 (1 byte) | datatype: 0 | factor_exp10: 1 | example: 2E23
  0x01,	 // 0x2F | moisture | uint8 (1 byte) | datatype: 0 | factor_exp10: 1 | example: 2F23
  0x00,	 // 0x30 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x31 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x32 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x33 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x34 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x35 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x36 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x37 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x38 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x39 | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x3A | TODO |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x3B | NA |  | datatype: 0 | factor_exp10:  | example: 
  0x00,	 // 0x3C | TODO |  | datatype: 0 | factor_exp10:  | example: 
  0x02,	 // 0x3D | count | uint (2 bytes) | datatype: 0 | factor_exp10: 1 | example: 3D0960
  0x04,	 // 0x3E | count | uint (4 bytes) | datatype: 0 | factor_exp10: 1 | example: 3E2A2C0960
  0x2A,	 // 0x3F | rotation | sint16 (2 bytes) | datatype: 1 | factor_exp10: 0.1 | example: 3F020C
  0x02,	 // 0x40 | distance (mm) | uint16 (2 bytes) | datatype: 0 | factor_exp10: 1 | example: 400C00
  0x22,	 // 0x41 | distance (m) | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.1 | example: 414E00
  0x63,	 // 0x42 | duration | uint24 (3 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 424E3400
  0x62,	 // 0x43 | current | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 434E34
  0x42,	 // 0x44 | speed | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.01 | example: 444E34
  0x2A,	 // 0x45 | temperature | sint16 (2 bytes) | datatype: 1 | factor_exp10: 0.1 | example: 451101
  0x21,	 // 0x46 | UV index | uint8 (1 byte) | datatype: 0 | factor_exp10: 0.1 | example: 4632
  0x22,	 // 0x47 | volume | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.1 | example: 478756
  0x02,	 // 0x48 | volume | uint16 (2 bytes) | datatype: 0 | factor_exp10: 1 | example: 48DC87
  0x62,	 // 0x49 | volume Flow Rate | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 49DC87
  0x22,	 // 0x4A | voltage | uint16 (2 bytes) | datatype: 0 | factor_exp10: 0.1 | example: 4A020C
  0x63,	 // 0x4B | gas | uint24 (3 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 4B138A14
  0x64,	 // 0x4C | gas | uint32 (4 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 4C41018A01
  0x64,	 // 0x4D | energy | uint32 (4 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 4d12138a14
  0x64,	 // 0x4E | volume | uint32 (4 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 4E87562A01
  0x64,	 // 0x4F | water | uint32 (4 bytes) | datatype: 0 | factor_exp10: 0.001 | example: 4F87562A01
};

float parse_integer(const uint8_t *data, HaBleTypes_e obj_data_format, uint8_t data_length)
{
  float value = {};
  bool is_signed = obj_data_format == HaBleType_sint;
  switch (data_length)
  {
    case 1:
      {
        uint16_t value1 = *data;
        return !is_signed ? value1 : signextend<int8_t,8>(value1);
      }
    case 2:
      {
        uint16_t value1 = combine_bytes_little_endian_u16(data);
        return !is_signed ? value1 : signextend<int16_t,16>(value1);
      }
    case 4:
      {
        uint32_t value1 = combine_bytes_little_endian_u32(data);
        return !is_signed ? value1 : signextend<int32_t,32>(value1);
      }
    default:
      return 0;
  }
}

// parse BTHome v1 protocol data - https://bthome.io/v1/ - UUID16 = 0x181C
// parse BTHome v2 protocol data - https://bthome.io/ - UUID16 = 0xfcd2
bool parse_payload_bthome(const uint8_t *payload_data, uint32_t payload_length, BTProtoVersion_e proto, measurement_cb_fn_t measurement_cb)
{
  uint8_t next_obj_start = 0;
  uint8_t prev_obj_meas_type = 0;
  uint8_t obj_meas_type;
  uint8_t obj_control_byte;
  uint8_t obj_data_length;
  uint8_t obj_data_format;
  uint8_t obj_data_start;
  float obj_data_factor;

  while (payload_length >= next_obj_start + 1)
  {
    auto obj_start = next_obj_start;

    if (proto == BTProtoVersion_BTHomeV1) {
      // BTHome V1
      obj_meas_type = payload_data[obj_start + 1];
      // obj_control_byte = payload_data[obj_start];
      // obj_data_length = (obj_control_byte >> 0) & 31; // 5 bits (0-4)
      // obj_data_format = (obj_control_byte >> 5) & 7;  // 3 bits (5-7)
      obj_data_start = obj_start + 2;

    } else if (proto == BTProtoVersion_BTHomeV2) {
      // BTHome V2
      if (prev_obj_meas_type > obj_meas_type) {
        ESP_LOGD(TAG, "BTHome device is not sending object ids in numerical order (from low to high object id).");
      }

      prev_obj_meas_type = obj_meas_type;
      obj_data_start = obj_start + 1;

    } else {
      ESP_LOGD(TAG, "BTHome unsupported protocol version %d.", proto);
      return false;
    }

    if (obj_meas_type >= sizeof(MEAS_TYPES_FLAGS)/sizeof(uint8_t)) {
      ESP_LOGD(TAG, "Invalid Object ID found in payload.");
      break;
    }
    const uint8_t meas_type_flags = pgm_read_byte_near(MEAS_TYPES_FLAGS + obj_meas_type);
    obj_data_length = meas_type_flags & 0b00000111;
    obj_data_format = meas_type_flags & 0b00011000;
    obj_data_factor = MEAS_TYPES_FACTORS[
                      meas_type_flags & 0b01100000
                      ];
    next_obj_start = obj_start + obj_data_length + 1;

    if (obj_data_length == 0) {
      ESP_LOGD(TAG, "Invalid payload data length found with length 0.");
      continue;
    }
    if (payload_length < next_obj_start) {
      ESP_LOGD(TAG, "Invalid payload data length.");
      break;
    } 

    const uint8_t obj_value_data_length = obj_data_length - (proto == BTProtoVersion_BTHomeV1 ? 1 : 0);

    const uint8_t *data = &payload_data[obj_data_start];
    float value = 0;
    if (obj_data_format == HaBleType_uint || obj_data_format == HaBleType_sint) {
        value = parse_integer(data, (HaBleTypes_e)obj_data_format, obj_value_data_length) * obj_data_factor;
    } else {
        ESP_LOGD(TAG, "Invalid payload data type %d.", obj_data_format);
        continue;
    }

    // report measurement
    if (measurement_cb) {
      measurement_cb(obj_meas_type, value);
    }
  }

  return true;
}

}