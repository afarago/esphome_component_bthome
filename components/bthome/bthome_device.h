/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"

#include "bthome_common.h"
#include "bthome_basesensor.h"
#include "bthome_sensor.h"

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

class BTHomeDevice: public Component
{
public:
  uint64_t get_address() { return this->address_; };
  void set_address(uint64_t address) { address_ = address; };
  DumpOption_e get_dump_option() { return this->dump_option_; };
  void set_dump_option(DumpOption_e value) { this->dump_option_ = value; };

  void dump_config() override;

  bool report_measurement_(uint8_t measurement_type, float value);

  bool match(const uint64_t mac_address) { 
    return (this->address_ == mac_address);
  }

  void register_sensor(esphome::bthome::BTHomeBaseSensor *sensor) {
    this->my_sensors.push_back(sensor);
  }

private:
  DumpOption_e dump_option_{DumpOption_None};
  uint64_t address_{0};

  std::vector<esphome::bthome::BTHomeBaseSensor *> my_sensors;
};

}
}

#endif