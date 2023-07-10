/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include "bthome_receiver_base_basesensor.h"

#include <vector>
#include <map>

namespace esphome
{
  namespace bthome_receiver_base
  {

    class BTHomeReceiverBaseBinarySensor : public binary_sensor::BinarySensor,
                                           public BTHomeReceiverBaseBaseSensor
    {
      void publish_data(float value) override
      {
        this->publish_state(value != 0.0); // TODO
      }
      const StringRef &get_name() const override
      {
        return binary_sensor::BinarySensor::get_name();
      }
    };
  }
}
