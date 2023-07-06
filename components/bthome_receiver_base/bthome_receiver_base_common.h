/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <string>
#include <functional>

#include "esphome/components/bthome_base/bthome_base_common.h"

namespace esphome
{
  namespace bthome_receiver_base
  {
    typedef enum
    {
      DumpOption_None = 0,
      DumpOption_Unmatched = 1,
      DumpOption_All = 2
    } DumpOption_e;

    typedef std::function<void(const bthome_base::bthome_measurement_record_t, const bool)> measurement_log_handler_t;
  }
}
