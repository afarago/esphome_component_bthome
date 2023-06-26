/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago

 */

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "esphome/components/bthome_base/bthome_base_common.h"
#include "esphome/components/bthome_base/bthome_encoder.h"

#include "esphome/components/beethowen_base/beethowen_base_common.h"
#include "esphome/components/beethowen_base/meshrc_bthome_over_espnow.h"

#include "esphome/components/beethowen_base/meshrc_bthome_over_espnow.h"

#include "beethowen_transmitter_hub.h"

using namespace std;

namespace esphome
{
  namespace beethowen_transmitter
  {
    static const char *const TAG = "beethowen_transmitter";
#define MAX_WIFI_CHANNELS 11

    void BeethowenTransmitterHub::setup()
    {
      // setup wifinow hooks
      beethowen_base::on_command([&](uint8_t command, uint8_t *buffer)
                                 { beethowen_on_command_(command, buffer); });

      ESP_LOGD(TAG, "Starting searching for Beethowen receiving server...");
    }

    void BeethowenTransmitterHub::beethowen_on_command_(uint8_t command, uint8_t *buffer)
    {
      ESP_LOGD(TAG, "Command received: %d, from: %s", command, bthome_base::addr_to_str(beethowen_base::sender).c_str());

      if (command == beethowen_base::BeethowenCommand_FoundServerResponse)
      {
        beethowen_base::beethowen_command_find_found_t *buffer2 = (beethowen_base::beethowen_command_find_found_t *)buffer;

        // validate remote passkey
        if (remote_expected_passkey_ != 0 && buffer2->passkey != remote_expected_passkey_)
        {
          ESP_LOGD(TAG, "Invalid remote found, passkey does not match expected passkey");
          // TODO: send error command with error code
          return;
        }

        set_server_address(bthome_base::addr_to_uint64(beethowen_base::sender));
        server_found_ = true;
        server_channel_ = buffer2->server_channel;

        ESP_LOGI(TAG, "Found server at {address} %s on {channel} %d with {passkey} %04X", bthome_base::addr64_to_str(server_address_).c_str(), buffer2->server_channel, buffer2->passkey);
      }
    }

    void BeethowenTransmitterHub::update()
    {
      if (!is_server_found())
      {
        if (initial_server_checkin_completed_ || server_channel_ == 0)
        {
          server_channel_++;
          if (server_channel_ > MAX_WIFI_CHANNELS)
            server_channel_ = 1;
        }
        else
        {
          initial_server_checkin_completed_ = true;
        }

        connect_to_wifi(server_channel_, connect_persistent_);

        ESP_LOGD(TAG, "trying to find server on {channel} %d, {local_passkey_} %04X, {remote_expected_passkey_} %04X", server_channel_, local_passkey_, remote_expected_passkey_);
        beethowen_base::send_find(beethowen_base::broadcast, server_channel_, local_passkey_);
      }
      else
      {
        // ESP_LOGD(TAG, "update {auto_send_} %d, {auto_send_done_} %d, {server_channel_} %d, ", auto_send_, auto_send_done_, server_channel_);
        if (this->auto_send_ && !this->auto_send_done_)
        {
          if (this->send(true))
            this->auto_send_done_ = true;
        }
      }
    }

    void BeethowenTransmitterHub::connect_to_wifi(uint8_t channel, bool persistent)
    {
      // ESP_LOGD(TAG, "connect_to_wifi channel {channel}, persistent {persistent}")
      WiFi.disconnect();
      beethowen_base::setupwifi(channel, persistent);
      beethowen_base::begin();
    }

    bool BeethowenTransmitterHub::send(bool do_complete_send)
    {
      bthome_base::BTHomeEncoder encoder;
      encoder.resetMeasurement();

      // collect data
      bool has_outstanding_measurements = false;
      for (auto btsensor_struct : my_sensors)
      {
        // ESP_LOGD(TAG, ".send {measurement_type} %d, {has_sensor_state} %d", btsensor_struct.measurement_type, has_sensor_state(btsensor_struct));
        if (has_sensor_state(btsensor_struct))
          encoder.addMeasurement(btsensor_struct.measurement_type, get_sensor_state(btsensor_struct).value());
        else
          has_outstanding_measurements = true;
      }
      if (do_complete_send && has_outstanding_measurements)
        return false;

      // ESP_LOGD(TAG, "send {do_complete_send} %d, {has_outstanding_measurements} %d", do_complete_send, has_outstanding_measurements);

      // perform send
      bool success = false;
      if (encoder.get_count() > 0)
      {
        uint8_t *bthome_data;
        uint8_t bthome_data_len;
        encoder.buildPaket(bthome_data, bthome_data_len);

        for (auto i = 0; i < 5; i++)
        {
          // ESP_LOGD(TAG, ". to: %s", bthome_base::addr_to_str(get_server_address_arr()).c_str());
          if (beethowen_base::send_with_header(get_server_address_arr(), bthome_data, bthome_data_len))
            beethowen_base::wait();
          // delayMicroseconds(10);

          if (beethowen_base::sending_success)
            break;
        }

        success = beethowen_base::sending_success;
      }

      if (!success)
      {
        ESP_LOGD(TAG, "ESPNow send failed, resetting server_found.");
        server_found_ = false; // invalidate server found
      }

      ESP_LOGD(TAG, "Sending finished {success} %d, {has_outstanding_measurements} %d, {count} %d",
               success,
               has_outstanding_measurements,
               encoder.get_count());
      if (success)
        this->on_send_finished_callback_.call(has_outstanding_measurements);
      if (success)
        this->on_send_failed_callback_.call();

      return success;
    }
  }
}
