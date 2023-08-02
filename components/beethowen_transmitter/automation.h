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
    class SendDataAction : public Action<Ts...>
    {
    public:
      explicit SendDataAction(BeethowenTransmitterHub *hub) : hub_(hub) {}

      void play(Ts... x) override { this->hub_->send_data(this->complete_only_); }

      void set_complete_only(bool value) { this->complete_only_ = value; }

    protected:
      BeethowenTransmitterHub *hub_;

    private:
      bool complete_only_{true};
    };

    template <typename... Ts>
    class SendEventAction : public Action<Ts...>
    {
    public:
      explicit SendEventAction(BeethowenTransmitterHub *hub) : hub_(hub) {}

      void play(Ts... x) override { this->hub_->send_event(this->device_type_, this->event_type_, this->value_); }

      void set_device_type(bool value) { this->device_type_ = value; }
      void set_event_type(bool value) { this->event_type_ = value; }
      void set_value(bool value) { this->value_ = value; }

    protected:
      BeethowenTransmitterHub *hub_;

    private:
      uint8_t device_type_{0x00};
      uint8_t event_type_{0x00};
      uint8_t value_{0x00};
    };

  }
}
