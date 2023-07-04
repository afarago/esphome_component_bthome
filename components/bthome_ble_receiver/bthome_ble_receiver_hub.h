/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

#include "esphome/components/bthome_base/bthome_base_common.h"
#include "esphome/components/bthome_receiver_base/bthome_receiver_base_hub.h"

#ifdef USE_ESP32

namespace esphome
{
  namespace bthome_ble_receiver
  {
    using namespace bthome_base;

#define MEASUREMENT_MAX_LEN_BTHOME_BLE 23 // 23=31(BLE_ADVERT_MAX_LEN)-3(FLAG)-1(SERVICE_DATA)-2(UUID)-1(ENCRYPT)-1(serviceData length bit)
    class BTHomeBLEReceiverHub : public esphome::bthome_receiver_base::BTHomeReceiverBaseHub,
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