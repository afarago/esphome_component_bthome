#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "bthome_parser.h"

#include "helpers.h"
#include "bthome_basesensor.h"
#include "bthome_sensor.h"
#include "bthome.h"

using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

static const char *const TAG = "bthome";

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
    BTProtoVersion_e proto = parse_header_(service_data);
    switch (proto) {
      case BTProtoVersion_BTHomeV1:
        if (parse_message_bthome_v1(service_data, device))
          success = true;
        break;
    }
  }

  return success;
}

BTProtoVersion_e BTHome::parse_header_(const esp32_ble_tracker::ServiceData &service_data)
{
  // 0000181c-0000-1000-8000-00805f9b34fb, 0000181e-0000-1000-8000-00805f9b34fb
  // esp32_ble_tracker::ESPBTUUID::from_uint16(0x181C)
  if (service_data.uuid.contains(0x1C, 0x18)) return BTProtoVersion_BTHomeV1; //unencrypted: 0000181c, encrypted: 0000181e
  // else if (service_data.uuid.contains(0xD2, 0xFC)) return BTProtoVersion_BTHomeV2; //0000fcd2
  else return BTProtoVersion_None;

  // auto raw = service_data.data;
  // static uint8_t last_frame_count = 0;
  // if (last_frame_count == raw[13]) {
  //   ESP_LOGVV(TAG, "parse_header(): duplicate data packet received (%hhu).", last_frame_count);
  //   return {};
  // }
  // last_frame_count = raw[13];
}

bool BTHome::parse_message_bthome_v1(const esp32_ble_tracker::ServiceData &service_data, const esp32_ble_tracker::ESPBTDevice &device)
{
  const std::vector<uint8_t> &message = service_data.data;
  const uint64_t address = device.address_uint64();
  const std::string name = device.get_name();

  // TODO: add v2 parsing later
  const uint8_t *payload_data = message.data();
  auto payload_length = message.size();

  return esphome::parse_payload_bthome_v1(payload_data, payload_length, [this, address, name](uint8_t measurement_type, float value) { 
    this->report_measurement_(measurement_type, value, address, name);
  });
}

// bool BTHome::parse_message_bthome_v2(const esp32_ble_tracker::ServiceData &service_data, const esp32_ble_tracker::ESPBTDevice &device)
// {
//   const std::vector<uint8_t> &message = service_data.data;
//   const uint64_t address = device.address_uint64();
//   const std::string name = device.get_name();

//   adv_info = data[0]
//   encryption = adv_info & (1 << 0)  # bit 0
//   mac_included = adv_info & (1 << 1)  # bit 1
//   sleepy_device = bool(adv_info & (1 << 2))  # bit 2
//   sw_version = (adv_info >> 5) & 7  # 3 bits (5-7)
  
//   if mac_included:
//       bthome_mac_reversed = data[1:7]
//       mac_readable = to_mac(bthome_mac_reversed[::-1])
//       payload = data[7:]
//   else:
//       mac_readable = service_info.address
//       payload = data[1:]

//   // TODO: add v2 parsing later
//   const uint8_t *payload_data = message.data();
//   auto payload_length = message.size();

//   return parse_payload_bthome_v1(payload_data, payload_length, [this, address, name](uint8_t measurement_type, float value) { 
//     this->report_measurement_(measurement_type, value, address, name);
//   });
// }

void BTHome::report_measurement_(uint8_t measurement_type, float value, uint64_t address, std::string name) {
    // got a measurement --> look for sensors, look for matches
    bool matched = false;
    for (auto btbase : this->my_sensors)
    {
      if (btbase->match(address, name, measurement_type))
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
      ESP_LOGD(TAG, "unmatched reading from %s, %s, measure_type: 0x%02x, value: %0.3f", 
        name.c_str(), addr_to_str(address).c_str(), measurement_type, value);
    }
#endif // ESPHOME_LOG_HAS_DEBUG
}


}
}

#endif