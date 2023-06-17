#pragma once

#include "esphome/core/component.h"

#include "bthome_basesensor.h"
#include "bthome_sensor.h"

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

class BTHomeDevice
{
public:
  uint64_t get_address() { return this->address_; };
  void set_address(uint64_t address) { address_ = address; };
  bool get_dump_unmatched_packages() { return  this->dump_unmatched_packages; };
  void set_dump_unmatched_packages(bool value) { dump_unmatched_packages = value; };

  void report_measurement_(uint8_t measurement_type, float value, bool global_dump_unmatched_packages);

  bool match(const uint64_t mac_address) { 
    return (this->address_ == mac_address);
  }

  void register_sensor(esphome::bthome::BTHomeBaseSensor *sensor) {
    this->my_sensors.push_back(sensor);
  }

private:
  bool dump_unmatched_packages;
  uint64_t address_{0};

  std::vector<esphome::bthome::BTHomeBaseSensor *> my_sensors;
};
}
}

#endif