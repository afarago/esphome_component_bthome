/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <vector>
#include <map>

#include "esphome/core/component.h"

#include "beethowen_transmitter_basesensor.h"

namespace esphome
{
  namespace beethowen_transmitter
  {
    class BeethowenTransmitterHub : public Component
    {
    public:
      inline uint64_t get_server_address() { return this->server_address_; };
      inline void set_server_address(uint64_t address) { server_address_ = address; };
      inline uint8_t get_server_channel() { return this->server_channel_; };
      inline void set_server_channel(uint8_t channel) { server_channel_ = channel; };

      void setup() override;
      void loop() override;

      float get_setup_priority() const override { return setup_priority::DATA; }

      void register_sensor(BeethowenTransmitterBaseSensor *sensor) { this->my_sensors.push_back(sensor); }

    protected:
      void beethowen_on_command_(uint8_t command);
      bool is_server_found() { return !!this->server_address_ && !!this->server_channel_; }
      void connect_to_wifi(uint8_t channel, bool persistent = false);

    private:
      uint64_t server_address_{0};
      uint8_t server_channel_{0};
      unsigned long last_find_millis = 0;

      std::vector<BeethowenTransmitterBaseSensor *> my_sensors;
    };

  }
}
