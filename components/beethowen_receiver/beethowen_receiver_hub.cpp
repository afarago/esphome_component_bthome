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

namespace esphome
{
  namespace beethowen_receiver
  {
    static const char *const TAG = "beethowen_receiver";

    void BeethowenReceiverHub::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up BeethowenReceiverHub...");

      beethowen_base::begin(true);

      // setup wifinow hooks
      beethowen_base::on_command([&](const uint8_t command, const uint8_t *buffer, const int size)
                                 { this->beethowen_on_command_(command, buffer, size); });
    }

    void BeethowenReceiverHub::beethowen_on_command_(const uint8_t command, const uint8_t *buffer, const int size)
    {
      auto client_mac_a64 = bthome_base::addr_to_uint64(beethowen_base::sender);
      auto device = static_cast<BeethowenReceiverDevice *>(get_device_by_address(client_mac_a64));
      auto buffer1 = (beethowen_base::beethowen_command_header_t *)buffer;

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
      ESP_LOGD(TAG, "Command received: 0x%02x, from: %s, device: %s",
               command,
               bthome_base::addr_to_str(beethowen_base::sender).c_str(),
               device ? device->get_name_prefix().c_str() : "<unknown>");
#endif // ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG

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
        auto buffer2 = (beethowen_base::beethowen_command_find_t *)buffer;
        uint8_t *client_mac = beethowen_base::sender;
        // TODO: consider sending the current wifi channel instead of the received one
        beethowen_base::send_command_find(client_mac, local_passkey_, buffer2->server_channel, false); // send ack
      }
      break;

      case beethowen_base::BeethowenCommand_Data:
      {
        // TODO: consider adding a separate processing queue on a thread
        auto buffer2 = (beethowen_base::beethowen_command_data_t *)buffer;
        auto payload_data = buffer2->data;
        auto payload_len = size - (BEETHOWEN_HEADER_LEN + BEETHOWEN_DATA_EXTRA_LEN);
        auto measurements = parse_message_bthome_(client_mac_a64, payload_data, payload_len, bthome_base::BTProtoVersion_BTHomeV2);

        // send ack
        if (this->send_data_ack_ && measurements.begin() != measurements.end())
        {
          // NOTE: could be an option to send byte[0] before parsing ang get packetid from byte[1]
          auto first_measurement = measurements.front();
          if (first_measurement.id == bthome_base::BTHOME_PACKET_ID_VALUE)
          {
            uint8_t *client_mac = beethowen_base::sender;
            beethowen_base::send_command_data_ack(client_mac, local_passkey_, (uint8_t)first_measurement.value);
          }
        }
      }
      break;

      default:
      {
        ESP_LOGW(TAG, "Unknown command type 0x%02x", command);
      }
      break;
      }
    }
  }
}
