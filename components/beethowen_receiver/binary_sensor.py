"""
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 """

from esphome.const import (
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_GAS,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_BATTERY_CHARGING,
    DEVICE_CLASS_COLD,
    DEVICE_CLASS_CONNECTIVITY,
    DEVICE_CLASS_DOOR,
    DEVICE_CLASS_GARAGE_DOOR,
    DEVICE_CLASS_HEAT,
    DEVICE_CLASS_LIGHT,
    DEVICE_CLASS_LOCK,
    DEVICE_CLASS_MOISTURE,
    DEVICE_CLASS_MOTION,
    DEVICE_CLASS_MOVING,
    DEVICE_CLASS_OCCUPANCY,
    DEVICE_CLASS_OPENING,
    DEVICE_CLASS_PLUG,
    DEVICE_CLASS_PRESENCE,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_RUNNING,
    DEVICE_CLASS_SAFETY,
    DEVICE_CLASS_SMOKE,
    DEVICE_CLASS_SOUND,
    DEVICE_CLASS_TAMPER,
    DEVICE_CLASS_VIBRATION,
    DEVICE_CLASS_WINDOW,
    DEVICE_CLASS_UPDATE,
)
from . import bthome_shared_sensor_configs

MEASUREMENT_TYPES_BINARY_SENSOR = {
    "generic_boolean": {"measurement_type": 0x0F},
    "power": {"measurement_type": 0x10, "device_class": DEVICE_CLASS_POWER},
    "opening": {"measurement_type": 0x11, "device_class": DEVICE_CLASS_OPENING},
    "battery": {"measurement_type": 0x15, "device_class": DEVICE_CLASS_BATTERY},
    "battery_charging": {
        "measurement_type": 0x16,
        "device_class": DEVICE_CLASS_BATTERY_CHARGING,
    },
    "carbon_monoxide": {"measurement_type": 0x17, "device_class": DEVICE_CLASS_GAS},
    "cold": {"measurement_type": 0x18, "device_class": DEVICE_CLASS_COLD},
    "connectivity": {
        "measurement_type": 0x19,
        "device_class": DEVICE_CLASS_CONNECTIVITY,
    },
    "door": {"measurement_type": 0x1A, "device_class": DEVICE_CLASS_DOOR},
    "garage_door": {"measurement_type": 0x1B, "device_class": DEVICE_CLASS_GARAGE_DOOR},
    "gas": {"measurement_type": 0x1C, "device_class": DEVICE_CLASS_GAS},
    "heat": {"measurement_type": 0x1D, "device_class": DEVICE_CLASS_HEAT},
    "light": {"measurement_type": 0x1E, "device_class": DEVICE_CLASS_LIGHT},
    "lock": {"measurement_type": 0x1F, "device_class": DEVICE_CLASS_LOCK},
    "moisture": {"measurement_type": 0x20, "device_class": DEVICE_CLASS_MOISTURE},
    "motion": {"measurement_type": 0x21, "device_class": DEVICE_CLASS_MOTION},
    "moving": {"measurement_type": 0x22, "device_class": DEVICE_CLASS_MOVING},
    "occupancy": {"measurement_type": 0x23, "device_class": DEVICE_CLASS_OCCUPANCY},
    "plug": {"measurement_type": 0x24, "device_class": DEVICE_CLASS_OPENING},
    "presence": {"measurement_type": 0x25, "device_class": DEVICE_CLASS_PLUG},
    "problem": {"measurement_type": 0x26, "device_class": DEVICE_CLASS_PRESENCE},
    "running": {"measurement_type": 0x27, "device_class": DEVICE_CLASS_PROBLEM},
    "safety": {"measurement_type": 0x28, "device_class": DEVICE_CLASS_RUNNING},
    "smoke": {"measurement_type": 0x29, "device_class": DEVICE_CLASS_SAFETY},
    "sound": {"measurement_type": 0x2A, "device_class": DEVICE_CLASS_SMOKE},
    "tamper": {"measurement_type": 0x2B, "device_class": DEVICE_CLASS_SOUND},
    "vibration": {"measurement_type": 0x2C, "device_class": DEVICE_CLASS_TAMPER},
    "window": {"measurement_type": 0x2D, "device_class": DEVICE_CLASS_VIBRATION},
}

CONFIG_SCHEMA, to_code = bthome_shared_sensor_configs(
    True, MEASUREMENT_TYPES_BINARY_SENSOR
)
