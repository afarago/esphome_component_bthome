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
  }
}
