/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago

 */

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "esphome/components/bthome_base/bthome_base_common.h"
#include "esphome/components/bthome_base/bthome_parser.h"

#include "esphome/components/beethowen_base/beethowen_base_common.h"
#include "esphome/components/beethowen_base/meshrc_bthome_over_espnow.h"

#include "beethowen_receiver_hub.h"
#include "beethowen_receiver_device.h"

using namespace std;

namespace esphome
{
  namespace beethowen_receiver
  {
    static const char *const TAG = "beethowen_receiver";

    void BeethowenReceiverHub::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up BeethowenReceiverHub...");

      beethowen_base::begin(); // call it only once

      // setup wifinow hooks
      beethowen_base::on_data([&](uint8_t *data, uint8_t size)
                              { this->beethowen_on_data_(data, size); });
      beethowen_base::on_command([&](uint8_t command, uint8_t *buffer)
                                 { this->beethowen_on_command_(command, buffer); });
    }

    void BeethowenReceiverHub::beethowen_on_command_(uint8_t command, uint8_t *buffer)
    {
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERBOSE
      ESP_LOGD(TAG, "Command received: %d, from: %s", command, bthome_base::addr_to_str(beethowen_base::sender).c_str());
#endif // ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERBOSE

      if (command == beethowen_base::BeethowenCommand_FindServerRequest)
      {
        auto buffer2 = (beethowen_base::beethowen_command_find_found_t *)buffer;
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
        ESP_LOGD(TAG, "Command received: %d, from: %s, channel: %d, passkey: %04X", command, bthome_base::addr_to_str(beethowen_base::sender).c_str(),
                 buffer2->server_channel, buffer2->passkey);
#endif // ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG

        auto client_mac_a64 = bthome_base::addr_to_uint64(beethowen_base::sender);
        // note: static_cast is OK as we only generate such objects in the init.py
        auto device = static_cast<BeethowenReceiverDevice *>(get_device_by_address(client_mac_a64));

        // validate remote passkey
        if (device && device->get_remote_expected_passkey() != 0 && buffer2->passkey != device->get_remote_expected_passkey())
        {
          ESP_LOGD(TAG, "Invalid remote found, passkey does not match expected passkey");
          // TODO: send error command with error code
          return;
        }

        uint8_t *client_mac = beethowen_base::sender;
        beethowen_base::send_command_found(client_mac, buffer2->server_channel, local_passkey_);
      }
    }

    void BeethowenReceiverHub::beethowen_on_data_(uint8_t *data, uint8_t size)
    {
      // identify device by mac address -- destination devices (everything under my_devices -> my_sensors)
      const uint64_t address = bthome_base::addr_to_uint64(beethowen_base::sender);

      parse_message_bthome_(address, data, size, bthome_base::BTProtoVersion_BTHomeV2);
    }
  }
}
