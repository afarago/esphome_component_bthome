/*
 BTHome V2 protocol packages encoder

 Based on BTHomeV2-ESP32-example DIY Sensor by Christos Baltatzidis
 https://github.com/Chreece/BTHomeV2-ESP32-example

 Author: Attila Farago
 */

#pragma once

#include <functional>
#include "bthome_base_common.h"
#include "bthome_common_generated.h"

namespace bthome_base
{

  using namespace std;

#define MAX_BUFFER_LEN 250u

  class BTHomeEncoder
  {
  public:
    BTHomeEncoder(uint16_t measurement_max_len_);

    uint8_t *buildPaket(uint8_t *&beethowen_data_out, uint8_t &beethowen_data_len_out);
    void resetMeasurement();
    bool addMeasurementEvent(bthome_measurement_t sensor_id, uint8_t state, uint8_t steps = 0);
    bool addMeasurementState(bthome_measurement_t sensor_id, bool state);
    bool addMeasurementValue(bthome_measurement_t sensor_id, uint64_t value);
    bool addMeasurementValue(bthome_measurement_t sensor_id, float value);
    inline bool addMeasurementValue(bthome_measurement_t sensor_id, uint8_t value) { return addMeasurementValue(sensor_id, (uint64_t)value); }
    inline bool addMeasurementValue(bthome_measurement_t sensor_id, uint16_t value) { return addMeasurementValue(sensor_id, (uint64_t)value); }
    inline bool addMeasurementValue(bthome_measurement_t sensor_id, uint32_t value) { return addMeasurementValue(sensor_id, (uint64_t)value); }
    uint8_t get_count() { return m_count; }

  protected:
    bool addMeasurementValueRaw(bthome_measurement_t sensor_id, uint64_t raw_value, uint8_t size);
    void addByteToMeasurement_(const uint8_t byte) { this->m_sensorData[this->m_sensorDataIdx++] = byte; };
    bool performAddMeasurement_(bthome_measurement_t sensor_id, uint8_t len, std::function<void()> executeCb);

  private:
    uint8_t m_sensorDataIdx;
    bthome_measurement_t m_sensorData[MAX_BUFFER_LEN] = {0};
    void sortSensorData();
    bool m_sortEnable;
    bthome_measurement_t last_object_id;
    uint8_t m_count{0};
    uint16_t measurement_max_len_{0};
  };

}
