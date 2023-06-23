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

#include "beethowen_transmitter_basesensor.h"
#include "beethowen_transmitter_sensor.h"
#include "beethowen_transmitter_hub.h"

using namespace std;

namespace esphome
{
  namespace beethowen_transmitter
  {
    static const char *const TAG = "beethowen_transmitter";

    void BeethowenTransmitterHub::setup()
    {
      // ESP_LOGCONFIG(TAG, "Setting up BeethowenTransmitterHub...");

      if (is_server_found())
      {
        // wifi_promiscuous_enable(1);
        // wifi_set_channel(id(server_channel));
        // wifi_promiscuous_enable(0);

        connect_to_wifi(server_channel_, connect_persistent_); 
      }
      else
      {
        ESP_LOGD(TAG, "Server not found and stored - starting searching for Beethowen receiving server");
      }

      // setup wifinow hooks
      beethowen_base::on_command([&](uint8_t command)
                                 { beethowen_on_command_(command); });
    }

    void BeethowenTransmitterHub::beethowen_on_command_(uint8_t command)
    {

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
      ESP_LOGD(TAG, "Command received: %d, from: %s", command, bthome_base::addr_to_str(beethowen_base::sender).c_str());
#endif // ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG

      if (command == beethowen_base::BeethowenCommand_FoundServerResponse)
      {
        set_server_address(bthome_base::addr_to_uint64(beethowen_base::sender));
        server_found_ = true;
        ESP_LOGI(TAG, "Found server at address %s on channel %d", bthome_base::addr64_to_str(server_address_).c_str(), server_channel_);
      }
    }

    void BeethowenTransmitterHub::loop()
    {
#define MAX_CHANNEL 11
#define FIND_TIMEOUT 100
      if (!is_server_found())
      {
        const uint32_t now = millis();

        if ((last_find_millis == 0) || (now - last_find_millis >= FIND_TIMEOUT))
        {
          server_channel_++;
          if (server_channel_ > MAX_CHANNEL)
            server_channel_ = 1;
          last_find_millis = now;

          connect_to_wifi(server_channel_, connect_persistent_); 

          ESP_LOGD(TAG, "trying to find server on channel %d", server_channel_);
          beethowen_base::send_find(beethowen_base::broadcast);
        }
      }
    }

    void BeethowenTransmitterHub::connect_to_wifi(uint8_t channel, bool persistent)
    {
      WiFi.disconnect();
      beethowen_base::setupwifi(channel, persistent);
      beethowen_base::begin();
    }

    void BeethowenTransmitterHub::transmit()
    {
      bthome_base::BTHomeEncoder encoder;
      encoder.resetMeasurement();

      for (auto btsensor : my_sensors)
      {
        if (!btsensor->is_binary())
        {
          BeethowenTransmitterSensor *btsensor_sensor = (BeethowenTransmitterSensor *)btsensor;
          if (btsensor_sensor)
          {
            if (btsensor_sensor->has_state())
            {
              // ESP_LOGD(TAG, "add sensor: %u.", btsensor->get_measurement_type());
              encoder.addMeasurement(btsensor->get_measurement_type(), btsensor_sensor->get_state());
            }
          }
        }
        // TODO: Add binary sensor
      }

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
          if (beethowen_base::sending_success)
            break;
          // delay(100);
        }

        success = beethowen_base::sending_success;
      }

      if (!success)
      {
        ESP_LOGV(TAG, "ESPNow send failed, resetting server_found.");

        // invalidate server found
        server_found_ = false;
      }
      else
      {
        ESP_LOGV(TAG, "Beethowen send success.");
      }
    }

  }
}
