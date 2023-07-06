/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "esphome/components/bthome_base/bthome_base_common.h"
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
    using namespace bthome_base;
    using namespace bthome_receiver_base;

    class BTHomeReceiverBaseHub : public Component
    {
    public:
      DumpOption_e get_dump_option() { return this->dump_option_; };
      void set_dump_option(DumpOption_e value) { this->dump_option_ = value; };
      void set_dump_packets_option(bool value) { this->dump_packets_option_ = value; };

      float get_setup_priority() const override { return setup_priority::DATA; }

      BTHomeReceiverBaseDevice *add_device(mac_address_t address);
      BTHomeReceiverBaseDevice *add_sensor(BTHomeReceiverBaseDevice *btdevice, mac_address_t address, BTHomeReceiverBaseBaseSensor *sensor);

      void add_on_packet_callback(std::function<void(const mac_address_t address, const vector<bthome_measurement_record_t> measurements)> callback)
      {
        this->on_packet_callback_.add(std::move(callback));
      }

    protected:
      CallbackManager<void(const mac_address_t address, const vector<bthome_measurement_record_t> measurements)> on_packet_callback_;

      virtual void parse_message_bthome_(const mac_address_t address, const uint8_t *payload_data, const uint32_t payload_length, bthome_base::BTProtoVersion_e proto);
      void report_measurements_(vector<bthome_measurement_record_t> measurements, mac_address_t address, BTHomeReceiverBaseDevice *btdevice);
      virtual BTHomeReceiverBaseDevice* create_device(const mac_address_t address) { return new BTHomeReceiverBaseDevice(address); }

      BTHomeReceiverBaseDevice *get_device_by_address(const mac_address_t address)
      {
        if (my_devices.find(address) != my_devices.end())
          return my_devices[address];
        else
          return nullptr;
      }

    private:
      DumpOption_e dump_option_{DumpOption_None};
      bool dump_packets_option_{false};
      std::map<mac_address_t, BTHomeReceiverBaseDevice *> my_devices;
    };
  }
}
