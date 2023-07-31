
/* auto generated, do not edit */

#pragma once
#include <pgmspace.h>

namespace bthome_base
{
typedef enum {
  BTHOME_PACKET_ID_VALUE = 0x00, 
  BTHOME_BATTERY_VALUE = 0x01, 
  BTHOME_TEMPERATURE_VALUE = 0x02, 
  BTHOME_HUMIDITY_VALUE = 0x03, 
  BTHOME_PRESSURE_VALUE = 0x04, 
  BTHOME_ILLUMINANCE_VALUE = 0x05, 
  BTHOME_MASS_KG_VALUE = 0x06, 
  BTHOME_MASS_LB_VALUE = 0x07, 
  BTHOME_DEWPOINT_VALUE = 0x08, 
  BTHOME_COUNT_VALUE = 0x09, 
  BTHOME_ENERGY_VALUE = 0x0a, 
  BTHOME_POWER_VALUE = 0x0b, 
  BTHOME_VOLTAGE_VALUE = 0x0c, 
  BTHOME_PM2_5_VALUE = 0x0d, 
  BTHOME_PM10_VALUE = 0x0e, 
  BTHOME_GENERIC_BOOLEAN_STATE = 0x0f, 
  BTHOME_POWER_STATE = 0x10, 
  BTHOME_OPENING_STATE = 0x11, 
  BTHOME_CO2_VALUE = 0x12, 
  BTHOME_TVOC_VALUE = 0x13, 
  BTHOME_MOISTURE_VALUE = 0x14, 
  BTHOME_BATTERY_STATE = 0x15, 
  BTHOME_BATTERY_CHARGING_STATE = 0x16, 
  BTHOME_CARBON_MONOXIDE_STATE = 0x17, 
  BTHOME_COLD_STATE = 0x18, 
  BTHOME_CONNECTIVITY_STATE = 0x19, 
  BTHOME_DOOR_STATE = 0x1a, 
  BTHOME_GARAGE_DOOR_STATE = 0x1b, 
  BTHOME_GAS_STATE = 0x1c, 
  BTHOME_HEAT_STATE = 0x1d, 
  BTHOME_LIGHT_STATE = 0x1e, 
  BTHOME_LOCK_STATE = 0x1f, 
  BTHOME_MOISTURE_STATE = 0x20, 
  BTHOME_MOTION_STATE = 0x21, 
  BTHOME_MOVING_STATE = 0x22, 
  BTHOME_OCCUPANCY_STATE = 0x23, 
  BTHOME_PLUG_STATE = 0x24, 
  BTHOME_PRESENCE_STATE = 0x25, 
  BTHOME_PROBLEM_STATE = 0x26, 
  BTHOME_RUNNING_STATE = 0x27, 
  BTHOME_SAFETY_STATE = 0x28, 
  BTHOME_SMOKE_STATE = 0x29, 
  BTHOME_SOUND_STATE = 0x2a, 
  BTHOME_TAMPER_STATE = 0x2b, 
  BTHOME_VIBRATION_STATE = 0x2c, 
  BTHOME_WINDOW_STATE = 0x2d, 
  BTHOME_HUMIDITY_COARSE_VALUE = 0x2e, 
  BTHOME_MOISTURE_COARSE_VALUE = 0x2f, 
  BTHOME_BUTTON_EVENT = 0x3a, 
  BTHOME_BUTTON_EXT_PRESS_EVENT = 0x3a, 
  BTHOME_BUTTON_EXT_DOUBLE_PRESS_EVENT = 0x3a, 
  BTHOME_BUTTON_EXT_TRIPLE_PRESS_EVENT = 0x3a, 
  BTHOME_BUTTON_EXT_LONG_PRESS_EVENT = 0x3a, 
  BTHOME_BUTTON_EXT_LONG_DOUBLE_PRESS_EVENT = 0x3a, 
  BTHOME_BUTTON_EXT_LONG_TRIPLE_PRESS_EVENT = 0x3a, 
  BTHOME_DIMMER_EVENT = 0x3c, 
  BTHOME_DIMMER_EXT_ROTATE_LEFT_EVENT = 0x3c, 
  BTHOME_DIMMER_EXT_ROTATE_RIGHT_EVENT = 0x3c, 
  BTHOME_COUNT_2_VALUE = 0x3d, 
  BTHOME_COUNT_4_VALUE = 0x3e, 
  BTHOME_ROTATION_VALUE = 0x3f, 
  BTHOME_DISTANCE_MM_VALUE = 0x40, 
  BTHOME_DISTANCE_M_VALUE = 0x41, 
  BTHOME_DURATION_VALUE = 0x42, 
  BTHOME_CURRENT_VALUE = 0x43, 
  BTHOME_SPEED_VALUE = 0x44, 
  BTHOME_TEMPERATURE_COARSE_VALUE = 0x45, 
  BTHOME_UV_INDEX_VALUE = 0x46, 
  BTHOME_VOLUME_VALUE = 0x47, 
  BTHOME_VOLUME_ML_VALUE = 0x48, 
  BTHOME_VOLUME_FLOW_RATE_VALUE = 0x49, 
  BTHOME_VOLTAGE_COARSE_VALUE = 0x4a, 
  BTHOME_GAS_VALUE = 0x4b, 
  BTHOME_GAS_4_VALUE = 0x4c, 
  BTHOME_ENERGY_4_VALUE = 0x4d, 
  BTHOME_VOLUME_PRECISE_VALUE = 0x4e, 
  BTHOME_WATER_VALUE = 0x4f, 
  BTHOME_TIMESTAMP_VALUE = 0x50, 
  BTHOME_ACCELERATION_VALUE = 0x51, 
  BTHOME_GYROSCOPE_VALUE = 0x52
} BTHome_e;

static const uint8_t PROGMEM MEAS_TYPES_FLAGS[] = { /* 8th bit Unused | 6-7th bits Factor | 4-5th bits DataType | 1-2-3rd bits DataLen */ 
  0b00000001, /* 0x00 | packet_id | packet_id | uint8 (1 byte) | 0 */
  0b00000001, /* 0x01 | battery | battery | uint8 (1 byte) | 0 */
  0b01001010, /* 0x02 | temperature | temperature | sint16 (2 bytes) | 2 */
  0b01000010, /* 0x03 | humidity | humidity | uint16 (2 bytes) | 2 */
  0b01000011, /* 0x04 | pressure | pressure | uint24 (3 bytes) | 2 */
  0b01000011, /* 0x05 | illuminance | illuminance | uint24 (3 bytes) | 2 */
  0b01000010, /* 0x06 | mass_kg | mass_kg | uint16 (2 byte) | 2 */
  0b01000010, /* 0x07 | mass_lb | mass_lb | uint16 (2 byte) | 2 */
  0b01001010, /* 0x08 | dewpoint | dewpoint | sint16 (2 bytes) | 2 */
  0b00000001, /* 0x09 | count | count | uint (1 bytes) | 0 */
  0b01100011, /* 0x0a | energy | energy | uint24 (3 bytes) | 3 */
  0b01000011, /* 0x0b | power | power | uint24 (3 bytes) | 2 */
  0b01100010, /* 0x0c | voltage | voltage | uint16 (2 bytes) | 3 */
  0b00000010, /* 0x0d | pm2_5 | pm2_5 | uint16 (2 bytes) | 0 */
  0b00000010, /* 0x0e | pm10 | pm10 | uint16 (2 bytes) | 0 */
  0b00000001, /* 0x0f | generic_boolean | generic_boolean | uint8 (1 byte) | 0 */
  0b00000001, /* 0x10 | power | power | uint8 (1 byte) | 0 */
  0b00000001, /* 0x11 | opening | opening | uint8 (1 byte) | 0 */
  0b00000010, /* 0x12 | co2 | co2 | uint16 (2 bytes) | 0 */
  0b00000010, /* 0x13 | tvoc | tvoc | uint16 (2 bytes) | 0 */
  0b01000010, /* 0x14 | moisture | moisture | uint16 (2 bytes) | 2 */
  0b00000001, /* 0x15 | battery | battery | uint8 (1 byte) | 0 */
  0b00000001, /* 0x16 | battery_charging | battery_charging | uint8 (1 byte) | 0 */
  0b00000001, /* 0x17 | carbon_monoxide | carbon_monoxide | uint8 (1 byte) | 0 */
  0b00000001, /* 0x18 | cold | cold | uint8 (1 byte) | 0 */
  0b00000001, /* 0x19 | connectivity | connectivity | uint8 (1 byte) | 0 */
  0b00000001, /* 0x1a | door | door | uint8 (1 byte) | 0 */
  0b00000001, /* 0x1b | garage_door | garage_door | uint8 (1 byte) | 0 */
  0b00000001, /* 0x1c | gas | gas | uint8 (1 byte) | 0 */
  0b00000001, /* 0x1d | heat | heat | uint8 (1 byte) | 0 */
  0b00000001, /* 0x1e | light | light | uint8 (1 byte) | 0 */
  0b00000001, /* 0x1f | lock | lock | uint8 (1 byte) | 0 */
  0b00000001, /* 0x20 | moisture | moisture | uint8 (1 byte) | 0 */
  0b00000001, /* 0x21 | motion | motion | uint8 (1 byte) | 0 */
  0b00000001, /* 0x22 | moving | moving | uint8 (1 byte) | 0 */
  0b00000001, /* 0x23 | occupancy | occupancy | uint8 (1 byte) | 0 */
  0b00000001, /* 0x24 | plug | plug | uint8 (1 byte) | 0 */
  0b00000001, /* 0x25 | presence | presence | uint8 (1 byte) | 0 */
  0b00000001, /* 0x26 | problem | problem | uint8 (1 byte) | 0 */
  0b00000001, /* 0x27 | running | running | uint8 (1 byte) | 0 */
  0b00000001, /* 0x28 | safety | safety | uint8 (1 byte) | 0 */
  0b00000001, /* 0x29 | smoke | smoke | uint8 (1 byte) | 0 */
  0b00000001, /* 0x2a | sound | sound | uint8 (1 byte) | 0 */
  0b00000001, /* 0x2b | tamper | tamper | uint8 (1 byte) | 0 */
  0b00000001, /* 0x2c | vibration | vibration | uint8 (1 byte) | 0 */
  0b00000001, /* 0x2d | window | window | uint8 (1 byte) | 0 */
  0b00000001, /* 0x2e | humidity | humidity_coarse | uint8 (1 byte) | 0 */
  0b00000001, /* 0x2f | moisture | moisture_coarse | uint8 (1 byte) | 0 */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000000, /* unused */
  0b00000001, /* 0x3a | button | button | uint8 (1 byte) | 0 */
  0b00000000, /* unused */
  0b00000010, /* 0x3c | dimmer | dimmer | uint8 (2 byte) | 0 */
  0b00000010, /* 0x3d | count | count_2 | uint (2 bytes) | 0 */
  0b00000100, /* 0x3e | count | count_4 | uint (4 bytes) | 0 */
  0b00101010, /* 0x3f | rotation | rotation | sint16 (2 bytes) | 1 */
  0b00000010, /* 0x40 | distance_mm | distance_mm | uint16 (2 bytes) | 0 */
  0b00100010, /* 0x41 | distance_m | distance_m | uint16 (2 bytes) | 1 */
  0b01100011, /* 0x42 | duration | duration | uint24 (3 bytes) | 3 */
  0b01100010, /* 0x43 | current | current | uint16 (2 bytes) | 3 */
  0b01000010, /* 0x44 | speed | speed | uint16 (2 bytes) | 2 */
  0b00101010, /* 0x45 | temperature | temperature_coarse | sint16 (2 bytes) | 1 */
  0b00100001, /* 0x46 | uv_index | uv_index | uint8 (1 byte) | 1 */
  0b00100010, /* 0x47 | volume | volume | uint16 (2 bytes) | 1 */
  0b00000010, /* 0x48 | volume | volume_mL | uint16 (2 bytes) | 0 */
  0b01100010, /* 0x49 | volume_flow_rate | volume_flow_rate | uint16 (2 bytes) | 3 */
  0b00100010, /* 0x4a | voltage | voltage_coarse | uint16 (2 bytes) | 1 */
  0b01100011, /* 0x4b | gas | gas | uint24 (3 bytes) | 3 */
  0b01100100, /* 0x4c | gas | gas_4 | uint32 (4 bytes) | 3 */
  0b01100100, /* 0x4d | energy | energy_4 | uint32 (4 bytes) | 3 */
  0b01100100, /* 0x4e | volume | volume_precise | uint32 (4 bytes) | 3 */
  0b01100100, /* 0x4f | water | water | uint32 (4 bytes) | 3 */
  0b00000100, /* 0x50 | timestamp | timestamp | uint48 (4 bytes) | 0 */
  0b01100010, /* 0x51 | acceleration | acceleration | uint16 (2 bytes) | 3 */
};

}
