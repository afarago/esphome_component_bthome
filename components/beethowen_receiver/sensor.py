"""
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 """

from esphome.const import (
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_AQI,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_CARBON_DIOXIDE,
    DEVICE_CLASS_CARBON_MONOXIDE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_GAS,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_ILLUMINANCE,
    DEVICE_CLASS_MONETARY,
    DEVICE_CLASS_NITROGEN_DIOXIDE,
    DEVICE_CLASS_NITROGEN_MONOXIDE,
    DEVICE_CLASS_NITROUS_OXIDE,
    DEVICE_CLASS_OZONE,
    DEVICE_CLASS_PM1,
    DEVICE_CLASS_PM10,
    DEVICE_CLASS_PM25,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_POWER_FACTOR,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_SULPHUR_DIOXIDE,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_TIMESTAMP,
    DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS,
    DEVICE_CLASS_VOLTAGE,
)
from . import beethowen_shared_sensor_configs

MEASUREMENT_TYPES_SENSOR = {
    "packet_id": {
        "measurement_type": 0x00,
        "accuracy_decimals": 0,
        "unit_of_measurement": "",
    },
    "battery": {
        "measurement_type": 0x01,
        "device_class": DEVICE_CLASS_BATTERY,
        "accuracy_decimals": 0,
        "unit_of_measurement": "%",
    },
    "temperature": {
        "measurement_type": 0x02,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "accuracy_decimals": 2,
        "unit_of_measurement": "°C",
    },
    "humidity": {
        "measurement_type": 0x03,
        "accuracy_decimals": 2,
        "unit_of_measurement": "%",
    },
    "pressure": {
        "measurement_type": 0x04,
        "device_class": DEVICE_CLASS_PRESSURE,
        "accuracy_decimals": 2,
        "unit_of_measurement": "hPa",
    },
    "illuminance": {
        "measurement_type": 0x05,
        "device_class": DEVICE_CLASS_ILLUMINANCE,
        "accuracy_decimals": 2,
        "unit_of_measurement": "lux",
    },
    "mass_kg": {
        "measurement_type": 0x06,
        "accuracy_decimals": 2,
        "unit_of_measurement": "kg",
    },
    "mass_lb": {
        "measurement_type": 0x07,
        "accuracy_decimals": 2,
        "unit_of_measurement": "lb",
    },
    "dewpoint": {
        "measurement_type": 0x08,
        "accuracy_decimals": 2,
        "unit_of_measurement": "°C",
    },
    "count_small": {
        "measurement_type": 0x09,
        "accuracy_decimals": 0,
        "unit_of_measurement": "",
    },
    "energy": {
        "measurement_type": 0x0A,
        "device_class": DEVICE_CLASS_ENERGY,
        "accuracy_decimals": 3,
        "unit_of_measurement": "kWh",
    },
    "power": {
        "measurement_type": 0x0B,
        "device_class": DEVICE_CLASS_POWER,
        "accuracy_decimals": 2,
        "unit_of_measurement": "W",
    },
    "voltage": {
        "measurement_type": 0x0C,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "accuracy_decimals": 3,
        "unit_of_measurement": "V",
    },
    "pm2_5": {
        "measurement_type": 0x0D,
        "device_class": DEVICE_CLASS_PM25,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ug/m3",
    },
    "pm10": {
        "measurement_type": 0x0E,
        "device_class": DEVICE_CLASS_PM10,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ug/m3",
    },
    "co2": {
        "measurement_type": 0x12,
        "device_class": DEVICE_CLASS_CARBON_DIOXIDE,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ppm",
    },
    "tvoc": {
        "measurement_type": 0x13,
        "device_class": DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ug/m3",
    },
    "moisture": {
        "measurement_type": 0x14,
        "accuracy_decimals": 2,
        "unit_of_measurement": "%",
    },
    "humidity_coarse": {
        "measurement_type": 0x2E,
        "accuracy_decimals": 0,
        "unit_of_measurement": "%",
    },
    "moisture_coarse": {
        "measurement_type": 0x2F,
        "accuracy_decimals": 0,
        "unit_of_measurement": "%",
    },
    "count_medium": {
        "measurement_type": 0x3D,
        "accuracy_decimals": 0,
        "unit_of_measurement": "",
    },
    "count_large": {
        "measurement_type": 0x3E,
        "accuracy_decimals": 0,
        "unit_of_measurement": "",
    },
    "rotation": {
        "measurement_type": 0x3F,
        "accuracy_decimals": 1,
        "unit_of_measurement": "°",
    },
    "distance_mm": {
        "measurement_type": 0x40,
        "accuracy_decimals": 0,
        "unit_of_measurement": "mm",
    },
    "distance_m": {
        "measurement_type": 0x41,
        "accuracy_decimals": 1,
        "unit_of_measurement": "m",
    },
    "duration": {
        "measurement_type": 0x42,
        "accuracy_decimals": 3,
        "unit_of_measurement": "s",
    },
    "current": {
        "measurement_type": 0x43,
        "device_class": DEVICE_CLASS_CURRENT,
        "accuracy_decimals": 3,
        "unit_of_measurement": "A",
    },
    "speed": {
        "measurement_type": 0x44,
        "accuracy_decimals": 2,
        "unit_of_measurement": "m/s",
    },
    "temperature_coarse": {
        "measurement_type": 0x45,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "accuracy_decimals": 1,
        "unit_of_measurement": "°C",
    },
    "UV_index": {
        "measurement_type": 0x46,
        "accuracy_decimals": 1,
        "unit_of_measurement": "",
    },
    "volume": {
        "measurement_type": 0x47,
        "accuracy_decimals": 1,
        "unit_of_measurement": "L",
    },
    "volume_course": {
        "measurement_type": 0x48,
        "accuracy_decimals": 0,
        "unit_of_measurement": "mL",
    },
    "volume_Flow_Rate": {
        "measurement_type": 0x49,
        "accuracy_decimals": 3,
        "unit_of_measurement": "m3/hr",
    },
    "voltage_coarse": {
        "measurement_type": 0x4A,
        "accuracy_decimals": 1,
        "unit_of_measurement": "V",
    },
    "gas": {
        "measurement_type": 0x4B,
        "device_class": DEVICE_CLASS_GAS,
        "accuracy_decimals": 3,
        "unit_of_measurement": "m3",
    },
    "gas_large": {
        "measurement_type": 0x4C,
        "device_class": DEVICE_CLASS_GAS,
        "accuracy_decimals": 3,
        "unit_of_measurement": "m3",
    },
    "energy_coarse": {
        "measurement_type": 0x4D,
        "device_class": DEVICE_CLASS_ENERGY,
        "accuracy_decimals": 3,
        "unit_of_measurement": "kWh",
    },
    "volume_precise": {
        "measurement_type": 0x4E,
        "accuracy_decimals": 3,
        "unit_of_measurement": "L",
    },
    "water": {
        "measurement_type": 0x4F,
        "accuracy_decimals": 3,
        "unit_of_measurement": "L",
    },
}

CONFIG_SCHEMA, to_code = beethowen_shared_sensor_configs(
    False, MEASUREMENT_TYPES_SENSOR
)
