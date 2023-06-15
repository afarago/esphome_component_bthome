#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

std::string addr_to_str(uint64_t address)
{
  std::string macstr = str_snprintf("%02x:%02x:%02x:%02x:%02x:%02x", 17, (uint8_t)(address >> 40) & 0xff,
                                    (uint8_t)(address >> 32) & 0xff, (uint8_t)(address >> 24) & 0xff,
                                    (uint8_t)(address >> 16) & 0xff, (uint8_t)(address >> 8) & 0xff,
                                    (uint8_t)(address >> 0) & 0xff);
  return macstr;
}

uint64_t addr_to_uint64(const uint8_t *address)
{
  uint64_t u = 0;
  u |= uint64_t(address[0] & 0xFF) << 40;
  u |= uint64_t(address[1] & 0xFF) << 32;
  u |= uint64_t(address[2] & 0xFF) << 24;
  u |= uint64_t(address[3] & 0xFF) << 16;
  u |= uint64_t(address[4] & 0xFF) << 8;
  u |= uint64_t(address[5] & 0xFF) << 0;
  return u;
}

union int16_u_t
{
  int16_t s;
  uint16_t u;
};
union int32_u_t
{
  int32_t s;
  uint32_t u;
};

uint16_t combine_bytes_little_endian_u16(const uint8_t *data) { return ((data[1] & 0xFF) << 8) | (data[0] & 0xFF); }
uint32_t combine_bytes_little_endian_u32(const uint8_t *data) { return ((data[3] & 0xFF) << 24) | ((data[2] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[0] & 0xFF); }
int16_t combine_bytes_little_endian_s16(const uint8_t *data)
{
  uint16_t val = combine_bytes_little_endian_u16(data);
  return static_cast<int16_u_t *>(static_cast<void *>(&val))->s;
}

int32_t combine_bytes_little_endian_s32(const uint8_t *data)
{
  uint32_t val = combine_bytes_little_endian_u32(data);
  return static_cast<int32_u_t *>(static_cast<void *>(&val))->s;
}

}
}

#endif