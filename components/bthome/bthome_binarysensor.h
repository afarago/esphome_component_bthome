/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include "bthome_basesensor.h"

#include <vector>
#include <map>
using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

class BTHomeBinarySensor : public binary_sensor::BinarySensor,
                            public BTHomeBaseSensor
{

  void publish_data(float value) override
  {
    this->publish_state(value != 0.0); // TODO
  };
};
}
}

#endif