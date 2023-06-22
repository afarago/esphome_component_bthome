/*
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#ifdef USE_ESP32

#include <string>
using namespace std;

namespace esphome {
namespace bthome {

typedef enum {
  DumpOption_None = 0,
  DumpOption_Unmatched = 1,
  DumpOption_All = 2
} DumpOption_e;

std::string addr_to_str(uint64_t address);
uint64_t addr_to_uint64(const uint8_t *address);

}
}

#endif