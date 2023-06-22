/*
 Beethowen BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "esphome/core/log.h"
#include "esphome/core/component.h"

#include "beethowen_receiver_common.h"
#include "beethowen_receiver_basesensor.h"

namespace esphome
{
  namespace beethowen_receiver
  {

    static const char *const TAG = "beethowen_receiver";

    void BeethowenReceiverBaseSensor::dump_config()
    {
      ESP_LOGCONFIG(TAG, "measurement_type 0x%X", this->measurement_type_);
    }

  }
}
