/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "bthome_receiver_base_common.h"
#include "bthome_receiver_base_device.h"
#include "bthome_receiver_base_basesensor.h"
#include "bthome_receiver_base_sensor.h"
#include "bthome_receiver_base_hub.h"

namespace esphome
{
  namespace bthome_receiver_base
  {
    static const char *const TAG = "bthome_receiver_base";

    void BTHomeReceiverBaseHub::report_measurement_(uint8_t measurement_type, float value, uint64_t address, BTHomeReceiverBaseDevice *btdevice, bool &device_header_reported)
    {
      bool matched = btdevice ? btdevice->report_measurement_(measurement_type, value) : false;

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
      // show in debug log any unmatched packages based on the dump_options
      if (
          (this->get_dump_option() == DumpOption_All || (!matched && (this->get_dump_option() == DumpOption_Unmatched))) ||
          (btdevice != NULL &&
           (btdevice->get_dump_option() == DumpOption_All || (!matched && (btdevice->get_dump_option() == DumpOption_Unmatched)))))
      {
        if (!device_header_reported)
        {
          ESP_LOGD(TAG, "Data received from %s", bthome_base::addr64_to_str(address).c_str());
          device_header_reported = true;
        }

        ESP_LOGD(TAG, " - measure_type: 0x%02x = value: %0.3f", measurement_type, value);
      }
#endif // ESPHOME_LOG_HAS_DEBUG
    }

    void BTHomeReceiverBaseHub::parse_message_bthome_(const uint64_t address, const uint8_t *payload_data, const uint32_t payload_length, bthome_base::BTProtoVersion_e proto)
    {
      auto *btdevice = get_device_by_address(address);
      if (!btdevice && this->get_dump_option() == DumpOption_None)
        return;

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERBOSE
      // log incoming packet
      std::string serviceData = "";
      for (auto i = 0; i < size; i++)
        serviceData += str_snprintf("%02X ", 3, data[i]);
      ESP_LOGV(TAG, "DATA received - %s = %s", bthome_base::addr64_to_str(beethowen_base::sender).c_str(), serviceData.c_str());
#endif // ESPHOME_LOG_LEVEL_VERBOSE

      // parse the payload and report measurements in the callback, will be fixing this to V2
      bool device_header_reported = false;
      bthome_base::parse_payload_bthome(
          payload_data, payload_length, proto,
          [&](uint8_t measurement_type, float value)
          {
            this->report_measurement_(measurement_type, value, address, btdevice, device_header_reported);
          },
          [&](const char *message)
          {
            ESP_LOGD(TAG, "%s", message);
          });
    }

  }
}
