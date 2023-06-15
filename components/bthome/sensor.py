import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
  CONF_ID, CONF_MAC_ADDRESS
)
from . import (
    bthome_ns, BTHome, CONF_BTHome_ID
)

DEPENDENCIES = ["bthome"]

CONF_MAC_ADDRESS = "mac_address"
CONF_MEASUREMENT_TYPE = "measurement_type"

MEASUREMENT_TYPES_SENSOR = {
    "packet_id": {"measurement_type": 0x00, "accuracy_decimals": 0, "unit_of_measurement":""},
    "battery": {"measurement_type": 0x01, "accuracy_decimals": 0, "unit_of_measurement":"%"},
    "temperature": {"measurement_type": 0x02, "accuracy_decimals": 2, "unit_of_measurement":"°C"},
    "humidity": {"measurement_type": 0x03, "accuracy_decimals": 2, "unit_of_measurement":"%"},
    "pressure": {"measurement_type": 0x04, "accuracy_decimals": 2, "unit_of_measurement":"hPa"},
    "illuminance": {"measurement_type": 0x05, "accuracy_decimals": 2, "unit_of_measurement":"lux"},
    "mass_kg": {"measurement_type": 0x06, "accuracy_decimals": 2, "unit_of_measurement":"kg"},
    "mass_lb": {"measurement_type": 0x07, "accuracy_decimals": 2, "unit_of_measurement":"lb"},
    "dewpoint": {"measurement_type": 0x08, "accuracy_decimals": 2, "unit_of_measurement":"°C"},
    "count": {"measurement_type": 0x09, "accuracy_decimals": 0, "unit_of_measurement":""},
    "energy": {"measurement_type": 0x0A, "accuracy_decimals": 3, "unit_of_measurement":"kWh"},
    "power": {"measurement_type": 0x0B, "accuracy_decimals": 2, "unit_of_measurement":"W"},
    "voltage": {"measurement_type": 0x0C, "accuracy_decimals": 3, "unit_of_measurement":"V"},
    "pm2_5": {"measurement_type": 0x0D, "accuracy_decimals": 0, "unit_of_measurement":"ug/m3"},
    "pm10": {"measurement_type": 0x0E, "accuracy_decimals": 0, "unit_of_measurement":"ug/m3"},
    "co2": {"measurement_type": 0x12, "accuracy_decimals": 0, "unit_of_measurement":"ppm"},
    "tvoc": {"measurement_type": 0x13, "accuracy_decimals": 0, "unit_of_measurement":"ug/m3"},
    "moisture": {"measurement_type": 0x14, "accuracy_decimals": 2, "unit_of_measurement":"%"},
    "humidity2": {"measurement_type": 0x2E, "accuracy_decimals": 0, "unit_of_measurement":"%"},
    "moisture": {"measurement_type": 0x2F, "accuracy_decimals": 0, "unit_of_measurement":"%"},
}

def _check_measurement_type(value):
    if isinstance(value, int):
        return value
    try:
        return int(value)
    except ValueError:
        pass
      
    if value in MEASUREMENT_TYPES_SENSOR:
      return MEASUREMENT_TYPES_SENSOR[value]

def validate_measurement_type(value):
    value = _check_measurement_type(value)
    return value

BTHomeSensor = bthome_ns.class_("BTHomeSensor", sensor.Sensor, cg.Component)

def validate_config(config):
    # todo: sould not allow binary_sensor if receiver_pin is not registered
    return config

CONFIG_SCHEMA = cv.All(
    sensor.SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(BTHomeSensor),
        cv.GenerateID(CONF_BTHome_ID): cv.use_id(BTHome),
        cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
        cv.Required(CONF_MEASUREMENT_TYPE): validate_measurement_type,
    }), 
    validate_config,
)

async def to_code(config):
    bthome_component = await cg.get_variable(config[CONF_BTHome_ID])

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    cg.add(var.set_address(config[CONF_MAC_ADDRESS].as_hex))
    
    if (isinstance(config[CONF_MEASUREMENT_TYPE], dict)):
      measurement_type_record = config[CONF_MEASUREMENT_TYPE]
      cg.add(var.set_measurement_type(measurement_type_record["measurement_type"]))
      cg.add(var.set_accuracy_decimals(measurement_type_record["accuracy_decimals"]))
      cg.add(var.set_unit_of_measurement(measurement_type_record["unit_of_measurement"]))
    else:
      cg.add(var.set_measurement_type(config[CONF_MEASUREMENT_TYPE]))
    
    cg.add(bthome_component.register_sensor(var))
