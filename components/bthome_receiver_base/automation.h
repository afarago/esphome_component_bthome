/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <vector>
#include <map>

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/bthome_base/bthome_base_common.h"

#include "bthome_receiver_base_hub.h"

namespace esphome
{
  namespace bthome_receiver_base
  {
    using namespace bthome_base;

    class PacketTrigger : public Trigger<const mac_address_t, const std::vector<bthome_measurement_record_t>>
    {
    public:
      explicit PacketTrigger(BTHomeReceiverBaseHub *parent)
      {
        parent->add_on_packet_callback([this](const mac_address_t address, const std::vector<bthome_measurement_record_t> measurements)
                                       { this->trigger(address, measurements); });
      }
    };

    class EventTrigger : public Trigger<const mac_address_t, const bthome_measurement_event_record_t>
    {
    public:
      explicit EventTrigger(BTHomeReceiverBaseHub *parent)
      {
        parent->add_on_event_callback([this](const mac_address_t address, const bthome_measurement_event_record_t event)
                                      { 
                                        if (
                                          (!this->device_type_ || this->device_type_ == event.device_type) && 
                                          (!this->event_type_ || this->event_type_ == event.event_type) 
                                          ) {
                                        this->trigger(address, event); 
                                      } });
      }

      void set_device_type(bthome_measurement_t value) { this->device_type_ = value; }
      void set_event_type(uint8_t value) { this->event_type_ = value; }

    private:
      uint8_t device_type_{0};
      uint8_t event_type_{0};
    };

    // add_on_event_callback
    //  if (event == "click" this->trigger(address, event);)

  }
}
