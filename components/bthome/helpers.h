using namespace std;

#ifdef USE_ESP32

namespace esphome {
namespace bthome {

std::string addr_to_str(uint64_t address);
uint64_t addr_to_uint64(const uint8_t *address);

}
}

#endif