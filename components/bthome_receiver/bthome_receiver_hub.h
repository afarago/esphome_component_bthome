/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

#include "esphome/components/bthome_receiver_base/bthome_receiver_base_hub.h"

#ifdef USE_ESP32

namespace esphome
{
  namespace bthome_receiver
  {

    class BTHomeReceiverHub : public esphome::bthome_receiver_base::BTHomeReceiverBaseHub,
                              public esp32_ble_tracker::ESPBTDeviceListener
    {
    public:
      bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;

    protected:
      bthome_base::BTProtoVersion_e parse_header_(const esp32_ble_tracker::ServiceData &service_data);
      bool parse_message_payload_(const esp32_ble_tracker::ServiceData &service_data, const esp32_ble_tracker::ESPBTDevice &device, bthome_base::BTProtoVersion_e proto);
    };

  }
}

#endif