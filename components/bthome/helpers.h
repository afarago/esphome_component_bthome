using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

std::string addr_to_str(uint64_t address);
uint64_t addr_to_uint64(const uint8_t *address);

uint16_t combine_bytes_little_endian_u16(const uint8_t *data);
uint32_t combine_bytes_little_endian_u32(const uint8_t *data);
int16_t combine_bytes_little_endian_s16(const uint8_t *data);
int32_t combine_bytes_little_endian_s32(const uint8_t *data);

}
}

#endif