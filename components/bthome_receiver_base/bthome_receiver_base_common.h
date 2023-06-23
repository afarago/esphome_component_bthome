/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <string>

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

  }
}
