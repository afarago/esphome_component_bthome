/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "esphome/core/log.h"
#include "esphome/core/component.h"

#include "bthome_receiver_base_common.h"
#include "bthome_receiver_base_basesensor.h"

namespace esphome
{
  namespace bthome_receiver_base
  {
    static const char *const TAG = "bthome_receiver_base";

    void BTHomeReceiverBaseBaseSensor::dump_config()
    {
      ESP_LOGCONFIG(TAG, "measurement_type 0x%X", this->measurement_type_);
    }

  }
}
