#pragma once

#include "esphome/core/component.h"

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

class BTHomeBaseSensor : public Component
{
public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void dump_config() override;

  void set_address(uint64_t address) { address_ = address; };
  void set_device_name(std::string device_name) { device_name_ = device_name; };
  void set_measurement_type(uint8_t measurement_type) { measurement_type_ = measurement_type; };

  bool match(const uint64_t mac_address, const std::string device_name, const uint8_t measurement_type) { 
    return 
      (this->address_ == 0 || this->address_ == mac_address) && 
      (this->device_name_.empty() || this->device_name_ == device_name) && 
      (this->measurement_type_ == measurement_type); 
    }

  virtual void publish_data(float value) = 0;

private:
  uint8_t measurement_type_{0};
  uint64_t address_{0};
  std::string device_name_{};
};
}
}

#endif