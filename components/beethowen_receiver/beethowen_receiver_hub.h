/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"

#include "esphome/components/bthome_base/bthome_base_common.h"
#include "esphome/components/bthome_receiver_base/bthome_receiver_base_hub.h"

#include "beethowen_receiver_device.h"

namespace esphome
{
  namespace beethowen_receiver
  {
    using namespace std;
    using namespace bthome_base;

    class BeethowenReceiverHub : public esphome::bthome_receiver_base::BTHomeReceiverBaseHub
    {
    public:
      inline void set_local_passkey(uint16_t value) { local_passkey_ = value; };

      void setup() override;

      float get_setup_priority() const override { return setup_priority::AFTER_CONNECTION; }

    protected:
      uint16_t local_passkey_{0};
      bool send_data_ack_{false};

      void beethowen_on_command_(const uint8_t command, const uint8_t *buffer, const int size);
      bthome_receiver_base::BTHomeReceiverBaseDevice* create_device(const mac_address_t address) override { return new BeethowenReceiverDevice(address); }
    };
  }
}
