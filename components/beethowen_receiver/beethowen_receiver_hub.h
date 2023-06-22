/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <vector>
#include <map>

#include "esphome/core/component.h"

#include "beethowen_receiver_common.h"
#include "beethowen_receiver_device.h"
#include "beethowen_receiver_basesensor.h"

namespace esphome
{
  namespace beethowen_receiver
  {

    class BeethowenReceiverHub : public Component
    {
    public:
      void setup() override;

      inline DumpOption_e get_dump_option() { return this->dump_option_; };
      inline void set_dump_option(DumpOption_e value) { this->dump_option_ = value; };

      float get_setup_priority() const override { return setup_priority::DATA; }

      void register_device(BeethowenReceiverDevice *btdevice)
      {
        this->my_devices.push_back(btdevice);
      }

      BeethowenReceiverDevice *register_sensor(BeethowenReceiverDevice *btdevice_in, uint64_t address, BeethowenReceiverBaseSensor *sensor)
      {

        // btdevice can be provied for sake of speed
        // if btdevice is not given then look for a matching device (by address)
        auto *btdevice = btdevice_in;
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
            btdevice = new BeethowenReceiverDevice();
            btdevice->set_address(address);
            this->register_device(btdevice);
          }
        }

        // register new btsensor for the btdevice
        btdevice->register_sensor(sensor);
        return btdevice;
      }

    protected:
      void report_measurement_(uint8_t measurement_type, float value, uint64_t address, BeethowenReceiverDevice *btdevice, bool &device_header_reported);
      void beethowen_on_data_(uint8_t *data, uint8_t size);
      void beethowen_on_command_(uint8_t command);

    private:
      DumpOption_e dump_option_{DumpOption_None};
      std::vector<BeethowenReceiverDevice *> my_devices;
    };

  }
}
