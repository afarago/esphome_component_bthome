/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"

#include "beethowen_receiver_common.h"
#include "beethowen_receiver_basesensor.h"
#include "beethowen_receiver_sensor.h"

namespace esphome
{
  namespace beethowen_receiver
  {

    class BeethowenReceiverDevice : public Component
    {
    public:
      inline uint64_t get_address() { return this->address_; };
      inline void set_address(uint64_t address) { address_ = address; };
      inline DumpOption_e get_dump_option() { return this->dump_option_; };
      inline void set_dump_option(DumpOption_e value) { this->dump_option_ = value; };

      void dump_config() override;

      bool report_measurement_(uint8_t measurement_type, float value);

      bool match(const uint64_t mac_address)
      {
        return (this->address_ == mac_address);
      }

      void register_sensor(BeethowenReceiverBaseSensor *sensor)
      {
        this->my_sensors.push_back(sensor);
      }

    private:
      DumpOption_e dump_option_{DumpOption_None};
      uint64_t address_{0};

      std::vector<BeethowenReceiverBaseSensor *> my_sensors;
    };

  }
}
