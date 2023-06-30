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
    BeethowenCommand_None = 0x00,
    BeethowenCommand_FindServerRequest = 0x01,
    BeethowenCommand_FoundServerResponse = 0x02,
    BeethowenCommand_Data = 0x16,
  } BeethowenCommand_e;
}
