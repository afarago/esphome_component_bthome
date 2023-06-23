/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/template/sensor/template_sensor.h"

#include "beethowen_transmitter_basesensor.h"

#include <vector>
#include <map>

namespace esphome
{
  namespace beethowen_transmitter
  {

    class BeethowenTransmitterSensor : public template_::TemplateSensor,
                                       public BeethowenTransmitterBaseSensor
    {
    public:
      float get_setup_priority() const override { return setup_priority::DATA; }
      void dump_config() override { BeethowenTransmitterBaseSensor::dump_config_base(); };

      bool is_binary() override { return false; };
    };
  }
}
