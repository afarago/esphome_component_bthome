#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

#include "bthome_parser.h"

#include "bthome_device.h"
#include "bthome_basesensor.h"
#include "bthome_binarysensor.h"
#include "bthome_sensor.h"

#include <vector>
#include <map>
using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

class BTHome : public Component, public esp32_ble_tracker::ESPBTDeviceListener
{
public:
  bool get_dump_unmatched_packages() { return  this->dump_unmatched_packages; };
  void set_dump_unmatched_packages(bool value) { dump_unmatched_packages = value; };

  bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void register_sensor(esphome::bthome::BTHomeBaseSensor *sensor) {
    auto address = sensor->get_address();

    BTHomeDevice *btdevice = NULL;
    for (auto btdevice_i : this->my_devices) {
      if (btdevice_i->match(address)) {
        btdevice = btdevice_i;
        break;
      }
    }

    if (!btdevice) {
      btdevice = new BTHomeDevice();
      btdevice->set_address(address);
      this->my_devices.push_back(btdevice);
    }

    btdevice->register_sensor(sensor);
  }

protected:
  BTProtoVersion_e parse_header_(const esp32_ble_tracker::ServiceData &service_data);
  bool parse_message_bthome_(const esp32_ble_tracker::ServiceData &service_data, const esp32_ble_tracker::ESPBTDevice &device, BTProtoVersion_e proto);

private:
  bool dump_unmatched_packages;
  std::vector<esphome::bthome::BTHomeDevice *> my_devices;
};

}
}

#endif