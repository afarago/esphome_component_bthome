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
      inline uint8_t *get_server_address_arr() { return this->server_address_arr_; };
      inline void set_server_address(uint64_t value)
      {
        server_address_ = value;

        server_address_arr_[0] = (uint8_t)(server_address_ >> 40) & 0xff;
        server_address_arr_[1] = (uint8_t)(server_address_ >> 32) & 0xff;
        server_address_arr_[2] = (uint8_t)(server_address_ >> 24) & 0xff;
        server_address_arr_[3] = (uint8_t)(server_address_ >> 16) & 0xff;
        server_address_arr_[4] = (uint8_t)(server_address_ >> 8) & 0xff;
        server_address_arr_[5] = (uint8_t)(server_address_ >> 0) & 0xff;
      };
      inline uint8_t get_server_channel() { return this->server_channel_; };
      inline void set_server_channel(uint8_t value) { server_channel_ = value; };
      inline uint8_t get_connect_persistent() { return this->connect_persistent_; };
      inline void set_connect_persistent(bool value) { connect_persistent_ = value; };

      void setup() override;
      void loop() override;

      float get_setup_priority() const override { return setup_priority::DATA; }

      void register_sensor(BeethowenTransmitterBaseSensor *sensor) { this->my_sensors.push_back(sensor); }

      void transmit();

    protected:
      void beethowen_on_command_(uint8_t command);
      bool is_server_found() { return this->server_found_; }
      void connect_to_wifi(uint8_t channel, bool persistent);

    private:
      uint64_t server_address_{0};
      uint8_t server_address_arr_[6];
      uint8_t server_channel_{0};
      bool server_found_{false};
      bool connect_persistent_{false};
      uint32_t last_find_millis = 0;

      std::vector<BeethowenTransmitterBaseSensor *> my_sensors;
    };
  }
}
