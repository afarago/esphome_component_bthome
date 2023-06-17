#include "helpers.h"
#include "bthome_device.h"

using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

static const char *const TAG = "bthome";

void BTHomeDevice::report_measurement_(uint8_t measurement_type, float value, bool global_dump_unmatched_packages) {

    // got a measurement --> look for matching sensors
    bool matched = false;
    for (auto btsensor : this->my_sensors) {
      if (btsensor->match(this->address_, measurement_type)) {
        btsensor->publish_data(value);
        matched = true;
      }
    }
 
// show in debug log any unmatched packages
#ifdef ESPHOME_LOG_HAS_DEBUG
    if (!matched && (this->dump_unmatched_packages || global_dump_unmatched_packages)) {
      ESP_LOGD(TAG, "unmatched reading from %s, measure_type: 0x%02x, value: %0.3f", 
        addr_to_str(this->address_).c_str(), measurement_type, value);
    }
#endif // ESPHOME_LOG_HAS_DEBUG
}

}
}

#endif