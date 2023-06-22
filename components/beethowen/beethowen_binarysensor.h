/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include "beethowen_basesensor.h"

#include <vector>
#include <map>

namespace esphome
{
  namespace beethowen
  {

    class BeethowenBinarySensor : public binary_sensor::BinarySensor,
                                  public BeethowenBaseSensor
    {

      void publish_data(float value) override
      {
        this->publish_state(value != 0.0); // TODO
      };
    };
  }
}
