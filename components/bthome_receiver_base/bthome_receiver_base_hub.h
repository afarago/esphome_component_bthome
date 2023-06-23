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

      float get_setup_priority() const override { return setup_priority::DATA; }

      void register_device(BTHomeReceiverBaseDevice *btdevice)
      {
        this->my_devices.push_back(btdevice);
      }

      BTHomeReceiverBaseDevice *register_sensor(BTHomeReceiverBaseDevice *btdevice_in, uint64_t address, BTHomeReceiverBaseBaseSensor *sensor)
      {
        // btdevice can be provied for sake of speed
        // if btdevice is not given then look for a matching device (by address)
        BTHomeReceiverBaseDevice *btdevice = btdevice_in;
        if (!btdevice)
        {
          for (auto btdevice_i : this->my_devices)
          {
            if (btdevice_i->match(address))
            {
              btdevice = btdevice_i;
              break;
            }
          }

          if (!btdevice)
          {
            btdevice = new BTHomeReceiverBaseDevice();
            btdevice->set_address(address);
            this->register_device(btdevice);
          }
        }

        // register new btsensor for the btdevice
        btdevice->register_sensor(sensor);
        return btdevice;
      }

    protected:
      virtual void parse_message_bthome_(const uint64_t address, const uint8_t *payload_data, const uint32_t payload_length, bthome_base::BTProtoVersion_e proto);
      virtual void report_measurement_(uint8_t measurement_type, float value, uint64_t address, BTHomeReceiverBaseDevice *btdevice, bool &device_header_reported);

      BTHomeReceiverBaseDevice *get_device_by_address(const uint64_t address)
      {
        for (auto btdevice_i : this->my_devices)
          if (btdevice_i->match(address))
            return btdevice_i;
        return nullptr;
      }

    private:
      DumpOption_e dump_option_{DumpOption_None};
      std::vector<BTHomeReceiverBaseDevice *> my_devices;
    };

  }
}
