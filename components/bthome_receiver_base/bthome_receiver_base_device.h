/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"

#include "esphome/components/bthome_base/bthome_base_common.h"

#include "bthome_receiver_base_common.h"
#include "bthome_receiver_base_basesensor.h"
#include "bthome_receiver_base_sensor.h"

namespace esphome
{
  namespace bthome_receiver_base
  {
    class BTHomeReceiverBaseDevice : public Component
    {
    public:
      BTHomeReceiverBaseDevice(bthome_base::mac_address_t address) { this->set_address(address); }

      bthome_base::mac_address_t get_address() { return this->address_; };
      void set_address(bthome_base::mac_address_t address) { address_ = address; };
      DumpOption_e get_dump_option() { return this->dump_option_; };
      void set_dump_option(DumpOption_e value) { this->dump_option_ = value; };
      std::string get_name_prefix() { return name_prefix_; };
      void set_name_prefix(std::string value) { name_prefix_ = value; };

      void dump_config() override;
      void report_measurements_(vector<bthome_base::bthome_measurement_record_t> measurements, measurement_log_handler_t measurement_log_handler_cb);
      bool match(const bthome_base::mac_address_t mac_address) { return (this->address_ == mac_address); }
      void register_sensor(BTHomeReceiverBaseBaseSensor *sensor) { this->my_sensors.push_back(sensor); }

    private:
      DumpOption_e dump_option_{DumpOption_None};
      bthome_base::mac_address_t address_{0};
      std::string name_prefix_{};

      std::vector<BTHomeReceiverBaseBaseSensor *> my_sensors;
    };

  }
}
