#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "helpers.h"
#include "bthome_basesensor.h"
#include "bthome_sensor.h"
#include "bthome.h"

#include <vector>
#include <map>
using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

static const char *const TAG = "bthome";

// Type bit 5-7
typedef enum
{
  HaBleType_uint = 0,
  HaBleType_sint = 1,
  HaBleType_float = 2,
  HaBleType_string = 3,
  HaBleType_MAC = 4
} HaBleTypes_e;

// union int16_u_t {
//     int16_t s;
//     uint16_t u;
// };
// union int32_u_t {
//     int32_t s;
//     uint32_t u;
// };

// uint16_t combine_bytes_little_endian_u16(const uint8_t *data) { return ((data[1] & 0xFF) << 8) | (data[0] & 0xFF); }
// uint32_t combine_bytes_little_endian_u32(const uint8_t *data) { return ((data[3] & 0xFF) << 24) | ((data[2] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[0] & 0xFF); }
// int16_t combine_bytes_little_endian_s16(const uint8_t *data) {
//     uint16_t val = combine_bytes_little_endian_u16(data);
//     return static_cast<int16_u_t*>(static_cast<void*>(&val))->s;
//   }
// int32_t combine_bytes_little_endian_s32(const uint8_t *data) {
//     uint32_t val = combine_bytes_little_endian_u32(data);
//     return static_cast<int32_u_t*>(static_cast<void*>(&val))->s;
//   }

bool BTHome::parse_device(const esp32_ble_tracker::ESPBTDevice &device)
{
  // if (device.address_uint64() != this->address_) {
  //   ESP_LOGVV(TAG, "parse_device(): unknown MAC address.");
  //   return false;
  // }
  // ESP_LOGVV(TAG, "parse_device(): MAC address %s found.", device.address_str().c_str());

  bool success = false;
  for (auto &service_data : device.get_service_datas())
  {
    if (!parse_header_(service_data))
      continue;
    if (!parse_message_(service_data.data, device.address_uint64()))
      continue;

    success = true;
  }

  return success;
}

bool BTHome::parse_header_(const esp32_ble_tracker::ServiceData &service_data)
{

  if (!service_data.uuid.contains(0x1C, 0x18))
  {
    // ESP_LOGD("bthome", "parse_header(): no service data UUID magic bytes.");
    return false;
  }
  // auto raw = service_data.data;

  // static uint8_t last_frame_count = 0;
  // if (last_frame_count == raw[13]) {
  //   ESP_LOGVV(TAG, "parse_header(): duplicate data packet received (%hhu).", last_frame_count);
  //   return {};
  // }
  // last_frame_count = raw[13];

  return true;
}

bool BTHome::parse_message_(const std::vector<uint8_t> &message, const uint64_t address)
{

  // parse BTHome v1 protocol data - https://bthome.io/v1/ - UUID16 = 0x181C
  // TODO: add v2 parsing later
  const uint8_t *payload_data = message.data();
  auto payload_length = message.size();

  auto next_obj_start = 0;
  while (payload_length >= next_obj_start + 1)
  {
    auto obj_start = next_obj_start;

    auto obj_meas_type = payload_data[obj_start + 1];
    auto obj_control_byte = payload_data[obj_start];
    auto obj_data_length = (obj_control_byte >> 0) & 31; // 5 bits (0-4)
    auto obj_data_format = (obj_control_byte >> 5) & 7;  // 3 bits (5-7)
    auto obj_data_start = obj_start + 2;
    next_obj_start = obj_start + obj_data_length + 1;

    auto factor =
        obj_meas_type >= 0x02 && obj_meas_type <= 0x08 || obj_meas_type == 0x0B || obj_meas_type == 0x14 ? 0.01 : 
        obj_meas_type == 0x0A || obj_meas_type == 0x0C ? 0.001
        : 1;

    auto obj_value_data_length = obj_data_length - 1;

    float value = 0.0;
    const uint8_t *data = &payload_data[obj_data_start];
    switch (obj_data_format)
    {
    case HaBleType_uint:
      switch (obj_value_data_length)
      {
      case 1:
        value = *data;
        break;
      case 2:
        value = combine_bytes_little_endian_u16(data);
        break;
      case 4:
        value = combine_bytes_little_endian_u32(data);
        break;
      }
      value = value * factor;
      break;

    case HaBleType_sint:
      switch (obj_value_data_length)
      {
      case 1:
        value = *data; //!!
        break;
      case 2:
        value = combine_bytes_little_endian_s16(data);
        break;
      case 4:
        value = combine_bytes_little_endian_s32(data);
        break;
      }
      value = value * factor;
      break;

      // TODO: handle other types as well (float, string, MAC)
    }

    // got a measurement --> look for sensors, look for matches
    bool matched = false;
    for (auto btbase : this->my_sensors)
    {
      if (btbase->match(address, obj_meas_type))
      {

        // auto btsensor = dynamic_cast<BTHomeSensor*>(btbase);
        btbase->publish_data(value);
        matched = true;
      }
    }

// show in debug log any unmatched packages
#ifdef ESPHOME_LOG_HAS_DEBUG
    if (!matched && dump_unmatched_packages)
    {
      ESP_LOGD(TAG, "unmatched reading from %s, measure_type: 0x%02x, value: %0.3f", addr_to_str(address).c_str(), obj_meas_type, value);
    }
#endif // ESPHOME_LOG_HAS_DEBUG
  }

  return true;
}

}
}

#endif