#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "bthome_basesensor.h"

#include <vector>
#include <map>
using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

class BTHomeSensor : public sensor::Sensor,
                      public BTHomeBaseSensor
{

  void publish_data(float value) override
  {
    this->publish_state(value);
  };
};

}
}

#endif