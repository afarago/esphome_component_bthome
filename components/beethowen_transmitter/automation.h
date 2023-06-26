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
#include "beethowen_transmitter_hub.h"

namespace esphome
{
  namespace beethowen_transmitter
  {
    class SendFinishedTrigger : public Trigger<bool>
    {
    public:
      explicit SendFinishedTrigger(BeethowenTransmitterHub *parent)
      {
        parent->add_on_send_finished_callback([this](bool has_outstanding_measurements)
                                              { this->trigger(has_outstanding_measurements); });
      }
    };
    class SendFailedTrigger : public Trigger<>
    {
    public:
      explicit SendFailedTrigger(BeethowenTransmitterHub *parent)
      {
        parent->add_on_send_failed_callback([this]()
                                            { this->trigger(); });
      }
    };

    template <typename... Ts>
    class SendAction : public Action<Ts...>
    {
    public:
      explicit SendAction(BeethowenTransmitterHub *hub) : hub_(hub) {}
      TEMPLATABLE_VALUE(bool, complete_send)

      void play(Ts... x) override { this->hub_->send(this->complete_send_.value(x...)); }

    protected:
      BeethowenTransmitterHub *hub_;
    };

  }
}
