/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"

#include "esphome/components/bthome_receiver_base/bthome_receiver_base_hub.h"
#include "esphome/components/bthome_receiver_base/bthome_receiver_base_device.h"

namespace esphome
{
  namespace beethowen_receiver
  {
    class BeethowenReceiverDevice : public esphome::bthome_receiver_base::BTHomeReceiverBaseDevice
    {
    public:
      inline void set_remote_expected_passkey(uint16_t value) { remote_expected_passkey_ = value; };
      inline uint16_t get_remote_expected_passkey() { return remote_expected_passkey_; };

    private:
      uint16_t remote_expected_passkey_{0};
    };

  }
}
