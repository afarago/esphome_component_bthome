/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <string>

namespace beethowen_base
{
  typedef enum
  {
    BeethowenCommand_None = 0,
    BeethowenCommand_FindServerRequest = 1,
    BeethowenCommand_FoundServerResponse = 2,
  } BeethowenCommand_e;
}
