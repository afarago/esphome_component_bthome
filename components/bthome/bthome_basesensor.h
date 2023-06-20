/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

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

  uint64_t get_measurement_type() { return this->measurement_type_; };
  void set_measurement_type(uint8_t measurement_type) { measurement_type_ = measurement_type; };

  bool match(const uint8_t measurement_type) { return this->measurement_type_ == measurement_type; }

  virtual void publish_data(float value) = 0;

private:
  uint8_t measurement_type_{0};
};
}
}

#endif