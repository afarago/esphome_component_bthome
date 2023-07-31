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
    class SendStartedTrigger : public Trigger<>
    {
    public:
      explicit SendStartedTrigger(BeethowenTransmitterHub *parent)
      {
        parent->add_on_send_started_callback([this]()
                                             { this->trigger(); });
      }
    };
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

      void play(Ts... x) override { this->hub_->send_data(this->complete_only_); }

      void set_complete_only(bool value) { this->complete_only_ = value; }

    protected:
      BeethowenTransmitterHub *hub_;

    private:
      bool complete_only_{true};
    };

  }
}
