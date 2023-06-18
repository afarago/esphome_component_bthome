#include "esphome/core/log.h"
#include "esphome/core/component.h"

#include "helpers.h"
#include "bthome_basesensor.h"

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

static const char *const TAG = "bthome";

void BTHomeBaseSensor::dump_config()
{
  ESP_LOGCONFIG(TAG, "measurement_type 0x%X", this->measurement_type_);
}

}
}

#endif