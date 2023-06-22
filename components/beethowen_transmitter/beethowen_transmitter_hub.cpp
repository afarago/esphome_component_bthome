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

#include "beethowen_transmitter_basesensor.h"
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

      if (this->is_server_found())
      {
        // wifi_promiscuous_enable(1);
        // wifi_set_channel(id(server_channel));
        // wifi_promiscuous_enable(0);

        this->connect_to_wifi(server_channel_, false); //!!
      }
      else
      {
        ESP_LOGD(TAG, "Server not found and stored - starting searching for Beethowen receiving server");
      }

      // setup wifinow hooks
      beethowen_base::on_command([&](uint8_t command)
                                 { this->beethowen_on_command_(command); });
    }

    void BeethowenTransmitterHub::beethowen_on_command_(uint8_t command)
    {

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
      ESP_LOGD(TAG, "Command received: %d, from: %s", command, bthome_base::addr_to_str(beethowen_base::sender).c_str());
#endif // ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG

      if (command == beethowen_base::BeethowenCommand_FoundServerResponse)
      {
        this->server_address_ = bthome_base::addr_to_uint64(beethowen_base::sender);
        ESP_LOGI(TAG, "Found server at address %s on channel %d", bthome_base::addr64_to_str(this->server_address_).c_str(), this->server_channel_);
      }
    }

    void BeethowenTransmitterHub::loop()
    {
#define MAX_CHANNEL 11
#define FIND_TIMEOUT 100
      if (!this->is_server_found())
      {
        auto currentMillis = millis();

        if ((this->last_find_millis == 0) || (currentMillis - this->last_find_millis >= FIND_TIMEOUT))
        {
          this->server_channel_++;
          if (this->server_channel_ > MAX_CHANNEL)
            this->server_channel_ = 1;
          this->last_find_millis = currentMillis;

          this->connect_to_wifi(server_channel_, false); //!!

          ESP_LOGD(TAG, "trying to find server on channel %d", this->server_channel_);
          beethowen_base::send_find(beethowen_base::broadcast);
        }
      }
    }

    void BeethowenTransmitterHub::connect_to_wifi(uint8_t channel, bool persistent)
    {
      WiFi.disconnect();
      beethowen_base::setupwifi(this->server_channel_, persistent);
      beethowen_base::begin();
    }

  }
}
