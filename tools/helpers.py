from esphome.components import sensor, binary_sensor

DEVICE_CLASS_BASIC = {
    "numeric": sensor.DEVICE_CLASSES,
    "binary": binary_sensor.DEVICE_CLASSES,
}

DEVICE_CLASS_HELPER = {
    "numeric": {
        "dewpoint": "moisture",
        "mass": "weight",
        "pm2_5": "pm25",
        "tvoc": "volatile_organic_compounds"
        # "tvoc": "nitrogen_dioxide" / "nitrogen_monoxide" / "nitrous_oxide" / "volatile_organic_compounds"
        # "pressure": "pressure" / "atmospheric_pressure"
        # "volume": "volume" / "volume_storage" / "water"
    },
    "binary": {},
}

BTHOME_ICONS = {
    0x00: "mdi:counter",
    0x01: "mdi:battery",
    0x02: "mdi:temperature-celsius",
    0x03: "mdi:percent",
    0x04: "mdi:gauge",
    0x05: "mdi:sun-wireless",
    0x06: "mdi:weight-kilogram",
    0x07: "mdi:weight-pound",
    0x08: "mdi:water-thermometer-outline",
    0x09: "mdi:counter",
    0x0A: "mdi:lightning-bolt",
    0x0B: "mdi:power",
    0x0C: "mdi:lightning-bolt",
    0x0D: None,
    0x0E: None,
    0x0F: None,
    0x10: "mdi:power",
    0x11: None,
    0x12: "mdi:molecule-co2",
    0x13: None,
    0x14: "mdi:percent",
    0x15: None,
    0x16: None,
    0x17: None,
    0x18: None,
    0x19: None,
    0x1A: None,
    0x1B: None,
    0x1C: None,
    0x1D: None,
    0x1E: None,
    0x1F: None,
    0x20: None,
    0x21: "mdi:motion-sensor",
    0x22: None,
    0x23: None,
    0x24: None,
    0x25: None,
    0x26: None,
    0x27: None,
    0x28: None,
    0x29: None,
    0x2A: None,
    0x2B: None,
    0x2C: None,
    0x2D: None,
    0x2E: "mdi:percent",
    0x2F: "mdi:percent",
    0x30: None,
    0x31: None,
    0x32: None,
    0x33: None,
    0x34: None,
    0x35: None,
    0x36: None,
    0x37: None,
    0x38: None,
    0x39: None,
    0x3A: None,
    0x3B: None,
    0x3C: None,
    0x3D: "mdi:counter",
    0x3E: "mdi:counter",
    0x3F: None,
    0x40: None,
    0x41: None,
    0x42: None,
    0x43: "mdi:current-ac",
    0x44: None,
    0x45: None,
    0x46: None,
    0x47: None,
    0x48: None,
    0x49: None,
    0x4A: None,
    0x4B: "mdi:gas-cylinder",
    0x4C: "mdi:gas-cylinder",
    0x4D: None,
    0x4E: None,
    0x4F: "mdi:water",
    0x50: None,
    0x51: "mdi:axis-arrow",
    0x52: None,
}


def find_matching_device_class(object_id, measurement_property, main_type):
    matching_classes = ([
        value
        for value in DEVICE_CLASS_BASIC[main_type]
        if value != sensor.DEVICE_CLASS_EMPTY and measurement_property.startswith(value)
    ] if main_type in DEVICE_CLASS_BASIC else []) + ([
        value
        for _, (key, value) in enumerate(DEVICE_CLASS_HELPER[main_type].items())
        if measurement_property.startswith(key)
    ] if main_type in DEVICE_CLASS_HELPER else [])

    device_class_picked = matching_classes[0] if len(
        matching_classes) else None
    # print(f"{measurement_property:>40} = {device_class_picked}")
    return device_class_picked


def find_matching_icon(object_id, measurement_property, main_type):
    return BTHOME_ICONS[object_id]

def msb(val):
    return int(hex(val)[2:4],16)
