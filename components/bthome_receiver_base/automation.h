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

#include "bthome_receiver_base_eventtarget.h"

namespace esphome
{
  namespace bthome_receiver_base
  {
    using namespace bthome_base;

    class PacketTrigger : public Trigger<const bthome_base::mac_address_t, const std::vector<bthome_base::bthome_measurement_record_t>>
    {
    public:
      explicit PacketTrigger(BTHomeReceiverBaseEventTarget *parent)
      {
        parent->add_on_packet_callback([this](const bthome_base::mac_address_t address, const std::vector<bthome_base::bthome_measurement_record_t> measurements)
                                       { 
                                          // call trigger if filters are unset or matching
                                          if (
                                            (!this->parent_device_address_ || this->parent_device_address_ == address)
                                          ) {
                                          this->trigger(address, measurements);
                                        } });
      }

      void set_parent_device_address(bthome_base::mac_address_t value) { parent_device_address_ = value; };

    private:
      bthome_base::mac_address_t parent_device_address_{0};
    };

    class EventTrigger : public Trigger<const bthome_base::mac_address_t, const bthome_base::bthome_measurement_event_record_t>
    {
    public:
      explicit EventTrigger(BTHomeReceiverBaseEventTarget *parent)
      {
        parent->add_on_event_callback([this](const bthome_base::mac_address_t address, const bthome_base::bthome_measurement_event_record_t event)
                                      { 
                                        // call trigger if filters are unset or matching
                                        if (
                                          (!this->device_type_ || this->device_type_ == event.device_type) && 
                                          (!this->event_type_ || this->event_type_ == event.event_type) &&
                                          (!this->parent_device_address_ || this->parent_device_address_ == address) 
                                          ) {
                                        this->trigger(address, event); 
                                      } });
      }

      void set_device_type(bthome_base::bthome_measurement_t value) { this->device_type_ = value; }
      void set_event_type(uint8_t value) { this->event_type_ = value; }
      void set_parent_device_address(bthome_base::mac_address_t value) { parent_device_address_ = value; };

    private:
      bthome_base::bthome_measurement_t device_type_{0};
      uint8_t event_type_{0};
      bthome_base::mac_address_t parent_device_address_{0};
    };
  }
}
