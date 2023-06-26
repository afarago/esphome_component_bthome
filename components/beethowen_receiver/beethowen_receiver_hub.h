/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"

#include "esphome/components/bthome_receiver_base/bthome_receiver_base_hub.h"

namespace esphome
{
  namespace beethowen_receiver
  {
    class BeethowenReceiverHub : public esphome::bthome_receiver_base::BTHomeReceiverBaseHub
    {
    public:
      inline void set_local_passkey(uint16_t value) { local_passkey_ = value; };
      inline void set_remote_expected_passkey(uint16_t value) { remote_expected_passkey_ = value; };

      void setup() override;

    protected:
      uint16_t local_passkey_{0};
      uint16_t remote_expected_passkey_{0};

      void beethowen_on_data_(uint8_t *data, uint8_t size);
      void beethowen_on_command_(uint8_t command, uint8_t *buffer);
    };
  }
}
