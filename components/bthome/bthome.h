#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

#include "bthome_basesensor.h"
#include "bthome_binarysensor.h"
#include "bthome_sensor.h"

#include <vector>
#include <map>
using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

typedef enum
{
  BTProtoVersion_None = 0,
  BTProtoVersion_BTHomeV1 = 1,
  BTProtoVersion_BTHomeV2 = 2,
} BTProtoVersion_e;

class BTHome : public Component, public esp32_ble_tracker::ESPBTDeviceListener
{
public:
  void set_dump_unmatched_packages(bool value) { dump_unmatched_packages = value; };

  bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void register_sensor(esphome::bthome::BTHomeBaseSensor *sensor)
  {
    this->my_sensors.push_back(sensor);
  }

protected:
  BTProtoVersion_e parse_header_(const esp32_ble_tracker::ServiceData &service_data);
  bool parse_message_bthome_v1(const esp32_ble_tracker::ServiceData &service_data, const esp32_ble_tracker::ESPBTDevice &device);
  void report_measurement_(uint8_t measurement_type, float value, uint64_t address, std::string name);

private:
  bool dump_unmatched_packages;
  std::vector<esphome::bthome::BTHomeBaseSensor *> my_sensors;
};

}
}

#endif