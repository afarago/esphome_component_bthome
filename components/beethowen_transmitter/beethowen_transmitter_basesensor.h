/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"

namespace esphome
{
  namespace beethowen_transmitter
  {
    class BeethowenTransmitterBaseSensor 
    {
    public:
      uint8_t get_measurement_type() { return this->measurement_type_; };
      void set_measurement_type(uint8_t measurement_type) { measurement_type_ = measurement_type; };
      virtual bool is_binary() = 0;

    protected:
      void dump_config_base();

    private:
      uint8_t measurement_type_{0};
    };
  }

}
