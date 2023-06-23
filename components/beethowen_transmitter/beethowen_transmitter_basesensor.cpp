/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "esphome/core/log.h"
#include "esphome/core/component.h"

#include "beethowen_transmitter_basesensor.h"

namespace esphome
{
  namespace beethowen_transmitter
  {
    static const char *const TAG = "beethowen_transmitter";

    void BeethowenTransmitterBaseSensor::dump_config_base()
    {
      ESP_LOGCONFIG(TAG, "measurement_type 0x%X", this->measurement_type_);
    }
  }
}
