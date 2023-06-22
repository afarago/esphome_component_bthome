/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <string>

namespace esphome
{
  namespace beethowen
  {

    typedef enum
    {
      DumpOption_None = 0,
      DumpOption_Unmatched = 1,
      DumpOption_All = 2
    } DumpOption_e;

    typedef enum
    {
      RoleOption_None = 0,
      RoleOption_Destination = 1,
      RoleOption_Source = 2,
      RoleOption_Both = RoleOption_Destination | RoleOption_Source,
    } RoleOption_e;

    typedef enum
    {
      BeethowenCommand_None = 0,
      BeethowenCommand_FindServerRequest = 1,
      BeethowenCommand_FoundServerResponse = 1,
    } BeethowenCommand_e;

    std::string addr64_to_str(uint64_t address);
    uint64_t addr_to_uint64(const uint8_t *address);
    std::string addr_to_str(const uint8_t *address);

  }
}
