/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/core/entity_base.h"
#include "esphome/components/bthome_base/bthome_base_common.h"

namespace esphome
{
  namespace bthome_receiver_base
  {
    class BTHomeReceiverBaseBaseSensor
    {
    public:
      bthome_base::bthome_measurement_t get_measurement_type() { return this->measurement_type_; };
      void set_measurement_type(bthome_base::bthome_measurement_t measurement_type) { measurement_type_ = measurement_type; };

      int compare(const bthome_base::bthome_measurement_t measurement_type) { return (this->measurement_type_ > measurement_type) - (this->measurement_type_ < measurement_type); }

      virtual void publish_data(float value) = 0;
      virtual const StringRef &get_name() const = 0;

    private:
      bthome_base::bthome_measurement_t measurement_type_{0};
    };
  }
}
