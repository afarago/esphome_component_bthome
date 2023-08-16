/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "esphome/components/bthome_base/bthome_base_common.h"

#include "bthome_receiver_base_device.h"

using namespace std;

namespace esphome
{
  namespace bthome_receiver_base
  {
    using namespace bthome_base;
    static const char *const TAG = "bthome_receiver_base.device";

    void BTHomeReceiverBaseDevice::dump_config()
    {
      ESP_LOGCONFIG(TAG, "device %s at 0x%llX", this->name_prefix_.c_str(), this->address_);
      ESP_LOGCONFIG(TAG, "  dump_option %d", this->dump_option_);
      for (auto sensor : my_sensors)
      {
        ESP_LOGCONFIG(TAG, "  - connected sensor: %s", sensor->get_name().c_str());
        ESP_LOGCONFIG(TAG, "    measurement_type: 0x%X", sensor->get_measurement_type());
      }
    }

    void BTHomeReceiverBaseDevice::report_measurements_(vector<bthome_measurement_record_t> measurements, measurement_log_handler_t measurement_log_handler_cb)
    {
      // got a measurement --> look for matching sensors and publish data
      // assumption: both incoming data and sensors are in increasing order, same measurement_id's are ordered appropriately
      // sensors are auto-arranged by esphome python

      // step 1. arrange incoming measurements if not in right order
      // if incoming data is unordered this will not match the sensors
      auto measurement_type_compare = ([](const bthome_measurement_record_t &a, const bthome_measurement_record_t &b)
                                       { return a.d.id < b.d.id; });

      if (!std::is_sorted(measurements.begin(), measurements.end(), measurement_type_compare))
      {
        ESP_LOGD(TAG, "BTHome device is not sending object ids in numerical order (from low to high object id).");
        std::stable_sort(measurements.begin(), measurements.end(), measurement_type_compare);
      }

      // step 2. iterate and match measurements with sensors
      auto btsensor_iter = this->my_sensors.begin();
      for (auto measurement : measurements)
      {
        bool matched = false;
        if (measurement.is_value)
        {
          while (btsensor_iter != this->my_sensors.end() && (*btsensor_iter)->compare(measurement.d.id) < 0)
            btsensor_iter = std::next(btsensor_iter);

          // check if sensor type is matches measurement type
          if (btsensor_iter != this->my_sensors.end() && (*btsensor_iter)->compare(measurement.d.id) == 0)
          {
            auto btsensor = *btsensor_iter;
            matched = true;
            btsensor->publish_data(measurement.d.value.value);
            btsensor_iter = std::next(btsensor_iter);
          }
        }
        else
        {
          // events are considered as matched
          matched = true;
        }

        // report
        if (measurement_log_handler_cb)
          measurement_log_handler_cb(measurement, matched);
      }
    }

  }
}
