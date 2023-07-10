/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "bthome_receiver_base_basesensor.h"

#include <vector>
#include <map>

namespace esphome
{
  namespace bthome_receiver_base
  {
    using namespace std;

    class BTHomeReceiverBaseSensor : public sensor::Sensor,
                                     public BTHomeReceiverBaseBaseSensor
    {
      void publish_data(float value) override
      {
        this->publish_state(value);
      }
      const StringRef &get_name() const override
      {
        return sensor::Sensor::get_name();
      }
    };
  }
}
