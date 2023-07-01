/*
 BTHome V2 protocol packages encoder

 Based on BTHomeV2-ESP32-example DIY Sensor by Christos Baltatzidis
 https://github.com/Chreece/BTHomeV2-ESP32-example

 Author: Attila Farago
 */

#pragma once

#include <functional>
#include "bthome_common_generated.h"

namespace bthome_base
{

  using namespace std;

#define MAX_BUFFER_LEN 250u
// #define MEASUREMENT_MAX_LEN_BTHOME_BLE 23 // 23=31(BLE_ADVERT_MAX_LEN)-3(FLAG)-1(SERVICE_DATA)-2(UUID)-1(ENCRYPT)-1(serviceData length bit)
// #define MEASUREMENT_MAX_LEN_WIFINOW_OVER_BTHOME MAX_BEETHOWEN_PAYLOAD_LENGTH// 250(ESP_NOW_MAX_LEN)-2(MAGIC_HEADER)-3(BEETHOWEN_MAGIC_DATA_EXTRA_HEADER_LEN == 1 device info, 2 passkey)

// #define STATE_ON 0x01
  // #define STATE_OFF 0x00

  // #define EVENT_BUTTON 0x3A
  // #define EVENT_BUTTON_NONE 0x00
  // #define EVENT_BUTTON_PRESS 0x01
  // #define EVENT_BUTTON_DOUBLE_PRESS 0x02
  // #define EVENT_BUTTON_TRIPLE_PRESS 0x03
  // #define EVENT_BUTTON_LONG_PRESS 0x04
  // #define EVENT_BUTTON_LONG_DOUBLE_PRESS 0x05
  // #define EVENT_BUTTTON_LONG_TRIPLE_PRESS 0x06

  // #define EVENT_DIMMER 0x3C
  // #define EVENT_DIMMER_NONE 0x00
  // #define EVENT_DIMMER_LEFT 0x01
  // #define EVENT_DIMMER_RIGHT 0x02

  class BTHomeEncoder
  {
  public:
    BTHomeEncoder(uint16_t measurement_max_len_);

    uint8_t *buildPaket(uint8_t *&beethowen_data_out, uint8_t &beethowen_data_len_out);
    void resetMeasurement();
    bool addMeasurementValueRaw(uint8_t sensor_id, uint64_t raw_value, uint8_t size);
    bool addMeasurement_state(uint8_t sensor_id, uint8_t state, uint8_t steps = 0);
    bool addMeasurementValue(uint8_t sensor_id, uint64_t value);
    bool addMeasurementValue(uint8_t sensor_id, float value);
    uint8_t get_count() { return m_count; }

  private:
    uint8_t m_sensorDataIdx;
    uint8_t m_sensorData[MAX_BUFFER_LEN] = {0};
    void sortSensorData();
    bool m_sortEnable;
    uint8_t last_object_id;
    uint8_t m_count{0};
    uint16_t measurement_max_len_{0};
  };

}
