
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
  BTHOME_DIMMER_EVENT = 0x3c, 
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

typedef enum {
  BTHOME_BUTTON_NONE = 0x00, 
  BTHOME_BUTTON_CLICK = 0x01, 
  BTHOME_BUTTON_DOUBLE_CLICK = 0x02, 
  BTHOME_BUTTON_TRIPLE_CLICK = 0x03, 
  BTHOME_BUTTON_LONG_CLICK = 0x04, 
  BTHOME_BUTTON_LONG_DOUBLE_CLICK = 0x05, 
  BTHOME_BUTTON_LONG_TRIPLE_CLICK = 0x06
} BTHome_Button_e;

typedef enum {
  BTHOME_DIMMER_NONE = 0x00, 
  BTHOME_DIMMER_ROTATE_LEFT = 0x01, 
  BTHOME_DIMMER_ROTATE_RIGHT = 0x02
} BTHome_Dimmer_e;

static const uint8_t PROGMEM MEAS_TYPES_FLAGS[] = { /* 8th bit Unused | 6-7th bits Factor | 4-5th bits DataType | 1-2-3rd bits DataLen */ 
  /* 0x00 */ 0b00000001, /* packet_id | uint8 (1 byte) | numeric * 1.0 */
  /* 0x01 */ 0b00000001, /* battery | uint8 (1 byte) | numeric * 1.0 */
  /* 0x02 */ 0b01001010, /* temperature | sint16 (2 bytes) | numeric * 0.01 */
  /* 0x03 */ 0b01000010, /* humidity | uint16 (2 bytes) | numeric * 0.01 */
  /* 0x04 */ 0b01000011, /* pressure | uint24 (3 bytes) | numeric * 0.01 */
  /* 0x05 */ 0b01000011, /* illuminance | uint24 (3 bytes) | numeric * 0.01 */
  /* 0x06 */ 0b01000010, /* mass_kg | uint16 (2 byte) | numeric * 0.01 */
  /* 0x07 */ 0b01000010, /* mass_lb | uint16 (2 byte) | numeric * 0.01 */
  /* 0x08 */ 0b01001010, /* dewpoint | sint16 (2 bytes) | numeric * 0.01 */
  /* 0x09 */ 0b00000001, /* count | uint (1 bytes) | numeric * 1.0 */
  /* 0x0a */ 0b01100011, /* energy | uint24 (3 bytes) | numeric * 0.001 */
  /* 0x0b */ 0b01000011, /* power | uint24 (3 bytes) | numeric * 0.01 */
  /* 0x0c */ 0b01100010, /* voltage | uint16 (2 bytes) | numeric * 0.001 */
  /* 0x0d */ 0b00000010, /* pm2_5 | uint16 (2 bytes) | numeric * 1.0 */
  /* 0x0e */ 0b00000010, /* pm10 | uint16 (2 bytes) | numeric * 1.0 */
  /* 0x0f */ 0b00000001, /* generic_boolean | uint8 (1 byte) | binary */
  /* 0x10 */ 0b00000001, /* power | uint8 (1 byte) | binary */
  /* 0x11 */ 0b00000001, /* opening | uint8 (1 byte) | binary */
  /* 0x12 */ 0b00000010, /* co2 | uint16 (2 bytes) | numeric * 1.0 */
  /* 0x13 */ 0b00000010, /* tvoc | uint16 (2 bytes) | numeric * 1.0 */
  /* 0x14 */ 0b01000010, /* moisture | uint16 (2 bytes) | numeric * 0.01 */
  /* 0x15 */ 0b00000001, /* battery | uint8 (1 byte) | binary */
  /* 0x16 */ 0b00000001, /* battery_charging | uint8 (1 byte) | binary */
  /* 0x17 */ 0b00000001, /* carbon_monoxide | uint8 (1 byte) | binary */
  /* 0x18 */ 0b00000001, /* cold | uint8 (1 byte) | binary */
  /* 0x19 */ 0b00000001, /* connectivity | uint8 (1 byte) | binary */
  /* 0x1a */ 0b00000001, /* door | uint8 (1 byte) | binary */
  /* 0x1b */ 0b00000001, /* garage_door | uint8 (1 byte) | binary */
  /* 0x1c */ 0b00000001, /* gas | uint8 (1 byte) | binary */
  /* 0x1d */ 0b00000001, /* heat | uint8 (1 byte) | binary */
  /* 0x1e */ 0b00000001, /* light | uint8 (1 byte) | binary */
  /* 0x1f */ 0b00000001, /* lock | uint8 (1 byte) | binary */
  /* 0x20 */ 0b00000001, /* moisture | uint8 (1 byte) | binary */
  /* 0x21 */ 0b00000001, /* motion | uint8 (1 byte) | binary */
  /* 0x22 */ 0b00000001, /* moving | uint8 (1 byte) | binary */
  /* 0x23 */ 0b00000001, /* occupancy | uint8 (1 byte) | binary */
  /* 0x24 */ 0b00000001, /* plug | uint8 (1 byte) | binary */
  /* 0x25 */ 0b00000001, /* presence | uint8 (1 byte) | binary */
  /* 0x26 */ 0b00000001, /* problem | uint8 (1 byte) | binary */
  /* 0x27 */ 0b00000001, /* running | uint8 (1 byte) | binary */
  /* 0x28 */ 0b00000001, /* safety | uint8 (1 byte) | binary */
  /* 0x29 */ 0b00000001, /* smoke | uint8 (1 byte) | binary */
  /* 0x2a */ 0b00000001, /* sound | uint8 (1 byte) | binary */
  /* 0x2b */ 0b00000001, /* tamper | uint8 (1 byte) | binary */
  /* 0x2c */ 0b00000001, /* vibration | uint8 (1 byte) | binary */
  /* 0x2d */ 0b00000001, /* window | uint8 (1 byte) | binary */
  /* 0x2e */ 0b00000001, /* humidity_coarse | uint8 (1 byte) | numeric * 1.0 */
  /* 0x2f */ 0b00000001, /* moisture_coarse | uint8 (1 byte) | numeric * 1.0 */
  /* 0x30 */ 0b00000000, /* unused */
  /* 0x31 */ 0b00000000, /* unused */
  /* 0x32 */ 0b00000000, /* unused */
  /* 0x33 */ 0b00000000, /* unused */
  /* 0x34 */ 0b00000000, /* unused */
  /* 0x35 */ 0b00000000, /* unused */
  /* 0x36 */ 0b00000000, /* unused */
  /* 0x37 */ 0b00000000, /* unused */
  /* 0x38 */ 0b00000000, /* unused */
  /* 0x39 */ 0b00000000, /* unused */
  /* 0x3a */ 0b00000001, /* button_none | uint8 (1 byte) | event */
  /* 0x3b */ 0b00000000, /* unused */
  /* 0x3c */ 0b00000010, /* dimmer_none | uint8 (2 byte) | event */
  /* 0x3d */ 0b00000010, /* count_2 | uint (2 bytes) | numeric * 1.0 */
  /* 0x3e */ 0b00000100, /* count_4 | uint (4 bytes) | numeric * 1.0 */
  /* 0x3f */ 0b00101010, /* rotation | sint16 (2 bytes) | numeric * 0.1 */
  /* 0x40 */ 0b00000010, /* distance_mm | uint16 (2 bytes) | numeric * 1.0 */
  /* 0x41 */ 0b00100010, /* distance_m | uint16 (2 bytes) | numeric * 0.1 */
  /* 0x42 */ 0b01100011, /* duration | uint24 (3 bytes) | numeric * 0.001 */
  /* 0x43 */ 0b01100010, /* current | uint16 (2 bytes) | numeric * 0.001 */
  /* 0x44 */ 0b01000010, /* speed | uint16 (2 bytes) | numeric * 0.01 */
  /* 0x45 */ 0b00101010, /* temperature_coarse | sint16 (2 bytes) | numeric * 0.1 */
  /* 0x46 */ 0b00100001, /* uv_index | uint8 (1 byte) | numeric * 0.1 */
  /* 0x47 */ 0b00100010, /* volume | uint16 (2 bytes) | numeric * 0.1 */
  /* 0x48 */ 0b00000010, /* volume_mL | uint16 (2 bytes) | numeric * 1.0 */
  /* 0x49 */ 0b01100010, /* volume_flow_rate | uint16 (2 bytes) | numeric * 0.001 */
  /* 0x4a */ 0b00100010, /* voltage_coarse | uint16 (2 bytes) | numeric * 0.1 */
  /* 0x4b */ 0b01100011, /* gas | uint24 (3 bytes) | numeric * 0.001 */
  /* 0x4c */ 0b01100100, /* gas_4 | uint32 (4 bytes) | numeric * 0.001 */
  /* 0x4d */ 0b01100100, /* energy_4 | uint32 (4 bytes) | numeric * 0.001 */
  /* 0x4e */ 0b01100100, /* volume_precise | uint32 (4 bytes) | numeric * 0.001 */
  /* 0x4f */ 0b01100100, /* water | uint32 (4 bytes) | numeric * 0.001 */
  /* 0x50 */ 0b00000100, /* timestamp | uint48 (4 bytes) | numeric * 1.0 */
  /* 0x51 */ 0b01100010, /* acceleration | uint16 (2 bytes) | numeric * 0.001 */
};

}
