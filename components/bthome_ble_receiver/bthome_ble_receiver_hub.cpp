/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/bthome_base/bthome_parser.h"

#include "bthome_ble_receiver_hub.h"

#ifdef USE_ESP32

namespace esphome
{
  namespace bthome_ble_receiver
  {
    static const char *const TAG = "bthome_ble_receiver";

    bool BTHomeBLEReceiverHub::parse_device(const esp32_ble_tracker::ESPBTDevice &device)
    {
      bool success = false;
      for (auto &service_data : device.get_service_datas())
      {
        bthome_base::BTProtoVersion_e proto = parse_header_(service_data);
        if (proto != bthome_base::BTProtoVersion_None)
        {
          if (parse_message_payload_(service_data, device, proto))
            success = true;
        }
      }

      return false;
    }

    bthome_base::BTProtoVersion_e BTHomeBLEReceiverHub::parse_header_(const esp32_ble_tracker::ServiceData &service_data)
    {
      // 0000181c-0000-1000-8000-00805f9b34fb, 0000181e-0000-1000-8000-00805f9b34fb
      // esp32_ble_tracker::ESPBTUUID::from_uint16(0x181C)
      if (service_data.uuid.contains(0x1C, 0x18))
        return bthome_base::BTProtoVersion_BTHomeV1; // unencrypted: 0000181c, encrypted: 0000181e
      else if (service_data.uuid.contains(0xD2, 0xFC))
        return bthome_base::BTProtoVersion_BTHomeV2; // 0000fcd2
      else
        return bthome_base::BTProtoVersion_None;

      // auto raw = service_data.data;
      // static uint8_t last_frame_count = 0;
      // if (last_frame_count == raw[13]) {
      //   ESP_LOGVV(TAG, "parse_header(): duplicate data packet received (%hhu).", last_frame_count);
      //   return {};
      // }
      // last_frame_count = raw[13];
    }

    bool BTHomeBLEReceiverHub::parse_message_payload_(const esp32_ble_tracker::ServiceData &service_data, const esp32_ble_tracker::ESPBTDevice &device, bthome_base::BTProtoVersion_e proto)
    {
      // Check and match the device
      const uint64_t address = device.address_uint64();

      // Parse the payload data
      const std::vector<uint8_t> &message = service_data.data;
      const uint8_t *payload_data = message.data();
      auto payload_length = message.size();

      if (proto == bthome_base::BTProtoVersion_BTHomeV1)
      {
        // NOOP
      }
      else if (proto == bthome_base::BTProtoVersion_BTHomeV2)
      {
        uint8_t adv_info = payload_data[0];
        // bool encryption = bool(adv_info & (1 << 0));       // bit 0
        bool mac_included = bool(adv_info & (1 << 1)); // bit 1
        // bool sleepy_device = bool(adv_info & (1 << 2));    // bit 2
        uint8_t sw_version = (adv_info >> 5) & 7; // 3 bits (5-7);

        if (proto != sw_version)
        {
          ESP_LOGD(TAG, "BTHome V2 device is using an unsupported sw_version %d.", sw_version);
          return false;
        }

        if (mac_included)
        {
          // bthome_receiver_mac_reversed = data[1:7]
          // mac_readable = to_mac(bthome_receiver_mac_reversed[::-1])
          payload_data += 7;
          payload_length = -7;
        }
        else
        {
          // mac_readable = service_info.address;
          payload_data += 1;
          payload_length = -1;
        }
      }

      // parse the payload and report measurements in the callback
      parse_message_bthome_(address, payload_data, payload_length, proto);

      return true; // TODO change parse_message_bthome_ to return bool
    }
  }
}

#endif