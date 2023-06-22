/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include "beethowen_transmitter_basesensor.h"

#include <vector>
#include <map>

namespace esphome
{
  namespace beethowen_transmitter
  {

    class BeethowenTransmitterBinarySensor : public binary_sensor::BinarySensor,
                                             public BeethowenTransmitterBaseSensor
    {
    };
  }
}
