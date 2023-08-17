/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "esphome/components/bthome_base/bthome_base_common.h"
#include "esphome/components/bthome_base/bthome_parser.h"

#include "bthome_receiver_base_common.h"
#include "bthome_receiver_base_device.h"
#include "bthome_receiver_base_basesensor.h"
#include "bthome_receiver_base_binarysensor.h"
#include "bthome_receiver_base_sensor.h"

#include <vector>
#include <map>

namespace esphome
{
  namespace bthome_receiver_base
  {
    using namespace std;
    using namespace bthome_base;
    using namespace bthome_receiver_base;

    class BTHomeReceiverBaseEventTarget
    {
    public:
      void add_on_packet_callback(std::function<void(const bthome_base::mac_address_t, const vector<bthome_base::bthome_measurement_record_t>)> callback)
      {
        this->on_packet_callback_.add(std::move(callback));
      }
      void add_on_event_callback(std::function<void(const bthome_base::mac_address_t, const bthome_base::bthome_measurement_event_record_t)> callback)
      {
        this->on_event_callback_.add(std::move(callback));
      }

    protected:
      CallbackManager<void(const bthome_base::mac_address_t, const vector<bthome_base::bthome_measurement_record_t>)> on_packet_callback_;
      CallbackManager<void(const bthome_base::mac_address_t, const bthome_base::bthome_measurement_event_record_t)> on_event_callback_;
    };
  }
}
