#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "bthome_parser.h"

#include "helpers.h"
#include "bthome_device.h"
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
  bool success = false;
  for (auto &service_data : device.get_service_datas())
  {
    BTProtoVersion_e proto = parse_header_(service_data);
    if (parse_message_bthome_(service_data, device, proto)) success = true;
  }

  return false;
}

BTProtoVersion_e BTHome::parse_header_(const esp32_ble_tracker::ServiceData &service_data)
{
  // 0000181c-0000-1000-8000-00805f9b34fb, 0000181e-0000-1000-8000-00805f9b34fb
  // esp32_ble_tracker::ESPBTUUID::from_uint16(0x181C)
  if (service_data.uuid.contains(0x1C, 0x18)) return BTProtoVersion_BTHomeV1;       // unencrypted: 0000181c, encrypted: 0000181e
  else if (service_data.uuid.contains(0xD2, 0xFC)) return BTProtoVersion_BTHomeV2;  // 0000fcd2
  else return BTProtoVersion_None;

  // auto raw = service_data.data;
  // static uint8_t last_frame_count = 0;
  // if (last_frame_count == raw[13]) {
  //   ESP_LOGVV(TAG, "parse_header(): duplicate data packet received (%hhu).", last_frame_count);
  //   return {};
  // }
  // last_frame_count = raw[13];
}

bool BTHome::parse_message_bthome_(const esp32_ble_tracker::ServiceData &service_data, const esp32_ble_tracker::ESPBTDevice &device, BTProtoVersion_e proto)
{
  // Check and match the device
  const uint64_t address = device.address_uint64();
  // const std::string name = device.get_name();

  BTHomeDevice *btdevice = NULL;
  for (auto btdevice_i : this->my_devices) {
    if (btdevice_i->match(address)) {
      btdevice = btdevice_i;
      break;
    }
  }
  if (!btdevice) return false;

  // Parse the payload data
  const std::vector<uint8_t> &message = service_data.data;
  const uint8_t *payload_data = message.data();
  auto payload_length = message.size();

  if (proto == BTProtoVersion_BTHomeV1) {
    // NOOP

  } else if (proto == BTProtoVersion_BTHomeV2) {
    uint8_t adv_info = payload_data[0];
    //bool encryption = bool(adv_info & (1 << 0));       // bit 0
    bool mac_included = bool(adv_info & (1 << 1));     // bit 1
    //bool sleepy_device = bool(adv_info & (1 << 2));    // bit 2
    uint8_t sw_version = (adv_info >> 5) & 7;          // 3 bits (5-7);

    if (proto != sw_version) {
      ESP_LOGD(TAG, "BTHome V2 device is using an unsupported sw_version %d.", sw_version);
      return false;
    }

    if (mac_included) {
        //bthome_mac_reversed = data[1:7]
        //mac_readable = to_mac(bthome_mac_reversed[::-1])
        payload_data += 7;
        payload_length =- 7;
    } else {
        //mac_readable = service_info.address;
        payload_data += 1;
        payload_length =- 1;
    }
  }

  // parse the payload and report measurements in the callback
  return esphome::parse_payload_bthome(payload_data, payload_length, proto,
    [this, address, btdevice](uint8_t measurement_type, float value) { 
      btdevice->report_measurement_(measurement_type, value, this->dump_unmatched_packages);
    }
  );
}

}
}

#endif