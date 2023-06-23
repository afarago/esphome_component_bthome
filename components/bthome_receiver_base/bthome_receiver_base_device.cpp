/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "bthome_receiver_base_device.h"

using namespace std;

namespace esphome
{
  namespace bthome_receiver_base
  {

    static const char *const TAG = "bthome_receiver_base";

    void BTHomeReceiverBaseDevice::dump_config()
    {
      ESP_LOGCONFIG(TAG, "address 0x%X", this->address_);
      ESP_LOGCONFIG(TAG, "dump_option %d", this->dump_option_);
    }

    bool BTHomeReceiverBaseDevice::report_measurement_(uint8_t measurement_type, float value)
    {

      // got a measurement --> look for matching sensors and publish data
      bool matched = false;
      for (auto btsensor : this->my_sensors)
      {
        if (btsensor->match(measurement_type))
        {
          btsensor->publish_data(value);
          matched = true;
        }
      }

      return matched;
    }

  }
}
