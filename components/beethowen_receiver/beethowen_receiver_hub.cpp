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
      beethowen_base::on_command([&](uint8_t command, uint8_t *buffer, uint8_t size)
                                 { this->beethowen_on_command_(command, buffer, size); });
    }

    void BeethowenReceiverHub::beethowen_on_command_(uint8_t command, uint8_t *buffer, uint8_t size)
    {
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERBOSE
      ESP_LOGD(TAG, "Command received: %d, from: %s", command, bthome_base::addr_to_str(beethowen_base::sender).c_str());
#endif // ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERBOSE

      auto client_mac_a64 = bthome_base::addr_to_uint64(beethowen_base::sender);
      auto device = static_cast<BeethowenReceiverDevice *>(get_device_by_address(client_mac_a64));
      auto buffer1 = (beethowen_base::beethowen_command_header_t *)buffer;

      // validate remote passkey
      if (device && device->get_remote_expected_passkey() != 0 && buffer1->passkey != device->get_remote_expected_passkey())
      {
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
        ESP_LOGD(TAG, "Invalid remote found, passkey does not match expected passkey - {passkey_received}: 0x%04X, {passkey_excpected}: 0x%04X",
                 buffer1->passkey, device->get_remote_expected_passkey());
#endif // ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
        return;
      }

      switch (command)
      {
      case beethowen_base::BeethowenCommand_FindServerRequest:
      {
        auto buffer2 = (beethowen_base::beethowen_command_find_found_t *)buffer;
        uint8_t *client_mac = beethowen_base::sender;
        beethowen_base::send_command_found(client_mac, buffer2->server_channel, local_passkey_);
      }
      break;

      case beethowen_base::BeethowenCommand_Data:
      {
        auto buffer2 = (beethowen_base::beethowen_command_data_t *)buffer;
        auto payload_data = buffer2->data;
        auto payload_len = size - (BEETHOWEN_HEADER_LEN + BEETHOWEN_DATA_EXTRA_LEN);
        parse_message_bthome_(client_mac_a64, payload_data, payload_len, bthome_base::BTProtoVersion_BTHomeV2);
      }
      break;

      default:
      {
        ESP_LOGD(TAG, "Unknown command type %d", command);
      }
      break;
      }
    }
  }
}
