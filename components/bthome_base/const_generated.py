MEASUREMENT_TYPES_BINARY_SENSOR = {
    "generic_boolean": {
        "measurement_type": 0x0f,
        "accuracy_decimals": 0
    },
    "power": {
        "measurement_type": 0x10,
        "accuracy_decimals": 0,
        "device_class": "power"
    },
    "opening": {
        "measurement_type": 0x11,
        "accuracy_decimals": 0,
        "device_class": "opening"
    },
    "battery": {
        "measurement_type": 0x15,
        "accuracy_decimals": 0,
        "device_class": "battery"
    },
    "battery_charging": {
        "measurement_type": 0x16,
        "accuracy_decimals": 0,
        "device_class": "battery"
    },
    "carbon_monoxide": {
        "measurement_type": 0x17,
        "accuracy_decimals": 0,
        "device_class": "carbon_monoxide"
    },
    "cold": {
        "measurement_type": 0x18,
        "accuracy_decimals": 0,
        "device_class": "cold"
    },
    "connectivity": {
        "measurement_type": 0x19,
        "accuracy_decimals": 0,
        "device_class": "connectivity"
    },
    "door": {
        "measurement_type": 0x1a,
        "accuracy_decimals": 0,
        "device_class": "door"
    },
    "garage_door": {
        "measurement_type": 0x1b,
        "accuracy_decimals": 0,
        "device_class": "garage_door"
    },
    "gas": {
        "measurement_type": 0x1c,
        "accuracy_decimals": 0,
        "device_class": "gas"
    },
    "heat": {
        "measurement_type": 0x1d,
        "accuracy_decimals": 0,
        "device_class": "heat"
    },
    "light": {
        "measurement_type": 0x1e,
        "accuracy_decimals": 0,
        "device_class": "light"
    },
    "lock": {
        "measurement_type": 0x1f,
        "accuracy_decimals": 0,
        "device_class": "lock"
    },
    "moisture": {
        "measurement_type": 0x20,
        "accuracy_decimals": 0,
        "device_class": "moisture"
    },
    "motion": {
        "measurement_type": 0x21,
        "accuracy_decimals": 0,
        "device_class": "motion"
    },
    "moving": {
        "measurement_type": 0x22,
        "accuracy_decimals": 0,
        "device_class": "moving"
    },
    "occupancy": {
        "measurement_type": 0x23,
        "accuracy_decimals": 0,
        "device_class": "occupancy"
    },
    "plug": {
        "measurement_type": 0x24,
        "accuracy_decimals": 0,
        "device_class": "plug"
    },
    "presence": {
        "measurement_type": 0x25,
        "accuracy_decimals": 0,
        "device_class": "presence"
    },
    "problem": {
        "measurement_type": 0x26,
        "accuracy_decimals": 0,
        "device_class": "problem"
    },
    "running": {
        "measurement_type": 0x27,
        "accuracy_decimals": 0,
        "device_class": "running"
    },
    "safety": {
        "measurement_type": 0x28,
        "accuracy_decimals": 0,
        "device_class": "safety"
    },
    "smoke": {
        "measurement_type": 0x29,
        "accuracy_decimals": 0,
        "device_class": "smoke"
    },
    "sound": {
        "measurement_type": 0x2a,
        "accuracy_decimals": 0,
        "device_class": "sound"
    },
    "tamper": {
        "measurement_type": 0x2b,
        "accuracy_decimals": 0,
        "device_class": "tamper"
    },
    "vibration": {
        "measurement_type": 0x2c,
        "accuracy_decimals": 0,
        "device_class": "vibration"
    },
    "window": {
        "measurement_type": 0x2d,
        "accuracy_decimals": 0,
        "device_class": "window"
    }
}
MEASUREMENT_TYPES_NUMERIC_SENSOR = {
    "packet_id": {
        "measurement_type": 0x00,
        "accuracy_decimals": 0
    },
    "battery": {
        "measurement_type": 0x01,
        "accuracy_decimals": 0,
        "unit_of_measurement": "%",
        "device_class": "battery"
    },
    "temperature": {
        "measurement_type": 0x02,
        "accuracy_decimals": 2,
        "unit_of_measurement": "°C",
        "device_class": "temperature"
    },
    "humidity": {
        "measurement_type": 0x03,
        "accuracy_decimals": 2,
        "unit_of_measurement": "%",
        "device_class": "humidity"
    },
    "pressure": {
        "measurement_type": 0x04,
        "accuracy_decimals": 2,
        "unit_of_measurement": "hPa",
        "device_class": "pressure"
    },
    "illuminance": {
        "measurement_type": 0x05,
        "accuracy_decimals": 2,
        "unit_of_measurement": "lux",
        "device_class": "illuminance"
    },
    "mass_kg": {
        "measurement_type": 0x06,
        "accuracy_decimals": 2,
        "unit_of_measurement": "kg",
        "device_class": "weight"
    },
    "mass_lb": {
        "measurement_type": 0x07,
        "accuracy_decimals": 2,
        "unit_of_measurement": "lb",
        "device_class": "weight"
    },
    "dewpoint": {
        "measurement_type": 0x08,
        "accuracy_decimals": 2,
        "unit_of_measurement": "°C",
        "device_class": "moisture"
    },
    "count": {
        "measurement_type": 0x09,
        "accuracy_decimals": 0,
        "unit_of_measurement": ""
    },
    "energy": {
        "measurement_type": 0x0a,
        "accuracy_decimals": 3,
        "unit_of_measurement": "kWh",
        "device_class": "energy"
    },
    "power": {
        "measurement_type": 0x0b,
        "accuracy_decimals": 2,
        "unit_of_measurement": "W",
        "device_class": "power"
    },
    "voltage": {
        "measurement_type": 0x0c,
        "accuracy_decimals": 3,
        "unit_of_measurement": "V",
        "device_class": "voltage"
    },
    "pm2_5": {
        "measurement_type": 0x0d,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ug/m3",
        "device_class": "pm25"
    },
    "pm10": {
        "measurement_type": 0x0e,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ug/m3",
        "device_class": "pm1"
    },
    "co2": {
        "measurement_type": 0x12,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ppm"
    },
    "tvoc": {
        "measurement_type": 0x13,
        "accuracy_decimals": 0,
        "unit_of_measurement": "ug/m3",
        "device_class": "volatile_organic_compounds"
    },
    "moisture": {
        "measurement_type": 0x14,
        "accuracy_decimals": 2,
        "unit_of_measurement": "%",
        "device_class": "moisture"
    },
    "humidity_coarse": {
        "measurement_type": 0x2e,
        "accuracy_decimals": 0,
        "unit_of_measurement": "%",
        "device_class": "humidity"
    },
    "moisture_coarse": {
        "measurement_type": 0x2f,
        "accuracy_decimals": 0,
        "unit_of_measurement": "%",
        "device_class": "moisture"
    },
    "count_2": {
        "measurement_type": 0x3d,
        "accuracy_decimals": 0,
        "unit_of_measurement": ""
    },
    "count_4": {
        "measurement_type": 0x3e,
        "accuracy_decimals": 0,
        "unit_of_measurement": ""
    },
    "rotation": {
        "measurement_type": 0x3f,
        "accuracy_decimals": 1,
        "unit_of_measurement": "°"
    },
    "distance_mm": {
        "measurement_type": 0x40,
        "accuracy_decimals": 0,
        "unit_of_measurement": "mm",
        "device_class": "distance"
    },
    "distance_m": {
        "measurement_type": 0x41,
        "accuracy_decimals": 1,
        "unit_of_measurement": "m",
        "device_class": "distance"
    },
    "duration": {
        "measurement_type": 0x42,
        "accuracy_decimals": 3,
        "unit_of_measurement": "s",
        "device_class": "duration"
    },
    "current": {
        "measurement_type": 0x43,
        "accuracy_decimals": 3,
        "unit_of_measurement": "A",
        "device_class": "current"
    },
    "speed": {
        "measurement_type": 0x44,
        "accuracy_decimals": 2,
        "unit_of_measurement": "m/s",
        "device_class": "speed"
    },
    "temperature_coarse": {
        "measurement_type": 0x45,
        "accuracy_decimals": 1,
        "unit_of_measurement": "°C",
        "device_class": "temperature"
    },
    "uv_index": {
        "measurement_type": 0x46,
        "accuracy_decimals": 1,
        "unit_of_measurement": ""
    },
    "volume": {
        "measurement_type": 0x47,
        "accuracy_decimals": 1,
        "unit_of_measurement": "L",
        "device_class": "volume"
    },
    "volume_mL": {
        "measurement_type": 0x48,
        "accuracy_decimals": 0,
        "unit_of_measurement": "mL",
        "device_class": "volume"
    },
    "volume_flow_rate": {
        "measurement_type": 0x49,
        "accuracy_decimals": 3,
        "unit_of_measurement": "m3/hr",
        "device_class": "volume"
    },
    "voltage_coarse": {
        "measurement_type": 0x4a,
        "accuracy_decimals": 1,
        "unit_of_measurement": "V",
        "device_class": "voltage"
    },
    "gas": {
        "measurement_type": 0x4b,
        "accuracy_decimals": 3,
        "unit_of_measurement": "m3",
        "device_class": "gas"
    },
    "gas_4": {
        "measurement_type": 0x4c,
        "accuracy_decimals": 3,
        "unit_of_measurement": "m3",
        "device_class": "gas"
    },
    "energy_4": {
        "measurement_type": 0x4d,
        "accuracy_decimals": 3,
        "unit_of_measurement": "kWh",
        "device_class": "energy"
    },
    "volume_precise": {
        "measurement_type": 0x4e,
        "accuracy_decimals": 3,
        "unit_of_measurement": "L",
        "device_class": "volume"
    },
    "water": {
        "measurement_type": 0x4f,
        "accuracy_decimals": 3,
        "unit_of_measurement": "L",
        "device_class": "water"
    },
    "timestamp": {
        "measurement_type": 0x50,
        "accuracy_decimals": 0,
        "unit_of_measurement": "",
        "device_class": "timestamp"
    },
    "acceleration": {
        "measurement_type": 0x51,
        "accuracy_decimals": 3,
        "unit_of_measurement": "m/s²"
    },
    "gyroscope": {
        "measurement_type": 0x52,
        "accuracy_decimals": 3,
        "unit_of_measurement": "°/s"
    }
}
