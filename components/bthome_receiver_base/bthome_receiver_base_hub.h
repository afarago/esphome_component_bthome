/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "esphome/components/bthome_base/bthome_parser.h"

#include "bthome_receiver_base_common.h"
#include "bthome_receiver_base_device.h"
#include "bthome_receiver_base_basesensor.h"
#include "bthome_receiver_base_binarysensor.h"
#include "bthome_receiver_base_sensor.h"

#include <vector>
#include <map>

namespace esphome
{
  namespace bthome_receiver_base
  {
    using namespace std;

    class BTHomeReceiverBaseHub : public Component
    {
    public:
      DumpOption_e get_dump_option() { return this->dump_option_; };
      void set_dump_option(DumpOption_e value) { this->dump_option_ = value; };
      void set_dump_packets_option(bool value) { this->dump_packets_option_ = value; };

      float get_setup_priority() const override { return setup_priority::DATA; }

      void register_device(uint64_t address, BTHomeReceiverBaseDevice *btdevice)
      {
        btdevice->set_address(address);
        my_devices.emplace(address, btdevice);
      }

      BTHomeReceiverBaseDevice *add_sensor(BTHomeReceiverBaseDevice *btdevice, uint64_t address, BTHomeReceiverBaseBaseSensor *sensor);

    protected:
      virtual void parse_message_bthome_(const uint64_t address, const uint8_t *payload_data, const uint32_t payload_length, bthome_base::BTProtoVersion_e proto);
      virtual void report_measurement_(uint8_t measurement_type, float value, uint64_t address, BTHomeReceiverBaseDevice *btdevice, bool &device_header_reported);

      BTHomeReceiverBaseDevice *get_device_by_address(const uint64_t address)
      {
        if (my_devices.find(address) != my_devices.end())
          return my_devices[address];
        else
          return nullptr;
      }

    private:
      DumpOption_e dump_option_{DumpOption_None};
      bool dump_packets_option_{false};
      std::map<uint64_t, BTHomeReceiverBaseDevice *> my_devices;
    };

  }
}
