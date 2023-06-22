/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <string>

namespace esphome
{
  namespace beethowen_receiver
  {
    typedef enum
    {
      DumpOption_None = 0,
      DumpOption_Unmatched = 1,
      DumpOption_All = 2
    } DumpOption_e;
  }
}
