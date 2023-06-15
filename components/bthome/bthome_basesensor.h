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
  void set_measurement_type(uint8_t measurement_type) { measurement_type_ = measurement_type; };

  bool match(const uint64_t mac_address, const uint8_t measurement_type) { return this->address_ == mac_address && this->measurement_type_ == measurement_type; }
  bool match(const uint8_t *mac_address, const uint8_t measurement_type);

  virtual void publish_data(float value) = 0;

private:
  uint8_t measurement_type_;
  uint64_t address_;
};
}
}

#endif