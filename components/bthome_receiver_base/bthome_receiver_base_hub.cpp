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

    BTHomeReceiverBaseDevice *BTHomeReceiverBaseHub::add_device(mac_address_t address)
    {
      auto btdevice = this->create_device(address);
      my_devices.emplace(address, btdevice);

      return btdevice;
    }

    BTHomeReceiverBaseDevice *BTHomeReceiverBaseHub::add_sensor(BTHomeReceiverBaseDevice *btdevice, mac_address_t address, BTHomeReceiverBaseBaseSensor *sensor)
    {
      if (!btdevice)
        btdevice = get_device_by_address(address);
      if (!btdevice)
        btdevice = add_device(address);

      // register new btsensor for the btdevice
      btdevice->register_sensor(sensor);
      return btdevice;
    }

    void BTHomeReceiverBaseHub::report_measurements_(vector<bthome_measurement_record_t> measurements, mac_address_t address, BTHomeReceiverBaseDevice *btdevice)
    {
      bool device_header_reported = false;
      measurement_log_handler_t measurement_log_handler = [&](const bthome_measurement_record_t measurement, const bool matched)
      {
#ifdef ESPHOME_LOG_HAS_DEBUG
        if (
            (this->get_dump_option() == DumpOption_All || (!matched && (this->get_dump_option() == DumpOption_Unmatched))) ||
            (btdevice != NULL &&
             (btdevice->get_dump_option() == DumpOption_All || (!matched && (btdevice->get_dump_option() == DumpOption_Unmatched)))))
        {
          if (!device_header_reported)
          {
            ESP_LOGD(TAG, "Data received from %s %s",
                     bthome_base::addr64_to_str(address).c_str(),
                     btdevice ? btdevice->get_name_prefix().c_str() : "");
            device_header_reported = true;
          }
          ESP_LOGD(TAG, " - measure_type: 0x%02x = value: %0.3f%s",
                   measurement.id, measurement.value,
                   matched ? "" : ", unmatched");
        }
#endif // ESPHOME_LOG_HAS_DEBUG
      };

      if (btdevice)
        btdevice->report_measurements_(measurements, measurement_log_handler);
      else
        for (auto item : measurements)
          measurement_log_handler(item, false);
    }

    void BTHomeReceiverBaseHub::parse_message_bthome_(const mac_address_t address, const uint8_t *payload_data, const uint32_t payload_length, bthome_base::BTProtoVersion_e proto)
    {
      // TODO: should do a loop here instead of finding the right device and stopping
      //  for (auto btdevice_i : this->my_devices)
      //    if (btdevice_i->match(address))
      //      return btdevice_i;

      auto *btdevice = get_device_by_address(address);
      if (!btdevice && this->get_dump_option() == DumpOption_None)
        return;

#ifdef ESPHOME_LOG_HAS_DEBUG
      if (dump_packets_option_)
      {
        // log incoming packet
        std::string serviceData = "";
        for (auto i = 0; i < payload_length; i++)
          serviceData += str_snprintf("%02X ", 3, payload_data[i]);
        ESP_LOGD(TAG, "DATA received - %s = %s", bthome_base::addr64_to_str(address).c_str(), serviceData.c_str());
      }
#endif // ESPHOME_LOG_LEVEL_DEBUG

      // parse the payload and report measurements in the callback, will be fixing this to V2
      vector<bthome_measurement_record_t> measurements;
      bthome_base::parse_payload_bthome(
          payload_data, payload_length, proto,
          [&](uint8_t measurement_type, float value)
          {
            measurements.push_back({measurement_type, value});
          },
          [&](const char *message)
          {
            ESP_LOGD(TAG, "%s", message);
          });

      // report the measurements
      this->report_measurements_(measurements, address, btdevice);

      // trigger automation
      this->on_packet_callback_.call(address, measurements);
    }

  }
}
