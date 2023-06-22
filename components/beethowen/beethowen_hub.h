/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <vector>
#include <map>

#include "esphome/core/component.h"

#include "beethowen_common.h"
#include "beethowen_device.h"
#include "beethowen_basesensor.h"

namespace esphome
{
  namespace beethowen
  {

    class BeethowenHub : public Component
    {
    public:
      void setup();

      inline DumpOption_e get_dump_option() { return this->dump_option_; };
      inline void set_dump_option(DumpOption_e value) { this->dump_option_ = value; };
      inline RoleOption_e get_role_option() { return this->role_option_; };
      inline void set_role_option(RoleOption_e value) { this->role_option_ = value; };

      float get_setup_priority() const override { return setup_priority::DATA; }

      void register_device(BeethowenDevice *btdevice)
      {
        this->my_devices.push_back(btdevice);
      }

      BeethowenDevice *register_sensor(BeethowenDevice *btdevice_in, uint64_t address, BeethowenBaseSensor *sensor)
      {

        // btdevice can be provied for sake of speed
        // if btdevice is not given then look for a matching device (by address)
        BeethowenDevice *btdevice = btdevice_in;
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
            btdevice = new BeethowenDevice();
            btdevice->set_address(address);
            this->register_device(btdevice);
          }
        }

        // register new btsensor for the btdevice
        btdevice->register_sensor(sensor);
        return btdevice;
      }

    protected:
      void report_measurement_(uint8_t measurement_type, float value, uint64_t address, BeethowenDevice *btdevice, bool &device_header_reported);
      void beethowen_on_data_(uint8_t *data, uint8_t size);
      void beethowen_on_command_(uint8_t command);

    private:
      DumpOption_e dump_option_{DumpOption_None};
      RoleOption_e role_option_{RoleOption_None};
      std::vector<BeethowenDevice *> my_devices;
    };

  }
}
