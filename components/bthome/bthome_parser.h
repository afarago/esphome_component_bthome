#pragma once

#include <functional>

using namespace std;

namespace esphome {

using measurement_cb_fn_t = std::function<void(uint8_t measurement_type, float value)>;

bool parse_payload_bthome_v1(const uint8_t *payload_data, uint32_t payload_length, measurement_cb_fn_t measurement_cb);

}