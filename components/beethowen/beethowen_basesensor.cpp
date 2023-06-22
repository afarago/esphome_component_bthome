/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "esphome/core/log.h"
#include "esphome/core/component.h"

#include "beethowen_common.h"
#include "beethowen_basesensor.h"

namespace esphome
{
  namespace beethowen
  {

    static const char *const TAG = "beethowen";

    void BeethowenBaseSensor::dump_config()
    {
      ESP_LOGCONFIG(TAG, "measurement_type 0x%X", this->measurement_type_);
    }

  }
}
