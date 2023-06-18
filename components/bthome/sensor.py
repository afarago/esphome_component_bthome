import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.util import Registry
from esphome.const import (
  CONF_ID, 
  CONF_NAME,
  CONF_MAC_ADDRESS,
  
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
from . import (
    bthome_ns, BTHome, CONF_BTHome_ID,
    CONF_DUMP_OPTION, DUMP_OPTION
)

DEPENDENCIES = ["bthome"]

CONF_SENSORS = "sensors"
CONF_MEASUREMENT_TYPE = "measurement_type"
CONF_NAME_PREFIX = "name_prefix"

MEASUREMENT_TYPES_SENSOR = {
  "packet_id": {"measurement_type": 0x00, "accuracy_decimals": 0, "unit_of_measurement": "", "device_class": DEVICE_CLASS_EMPTY},
  "battery": {"measurement_type": 0x01, "accuracy_decimals": 0, "unit_of_measurement": "%", "device_class": DEVICE_CLASS_BATTERY},
  "temperature": {"measurement_type": 0x02, "accuracy_decimals": 2, "unit_of_measurement": "°C", "device_class": DEVICE_CLASS_TEMPERATURE},
  "humidity": {"measurement_type": 0x03, "accuracy_decimals": 2, "unit_of_measurement": "%", "device_class": DEVICE_CLASS_HUMIDITY},
  "pressure": {"measurement_type": 0x04, "accuracy_decimals": 2, "unit_of_measurement": "hPa", "device_class": DEVICE_CLASS_PRESSURE},
  "illuminance": {"measurement_type": 0x05, "accuracy_decimals": 2, "unit_of_measurement": "lux", "device_class": DEVICE_CLASS_ILLUMINANCE},
  "mass_kg": {"measurement_type": 0x06, "accuracy_decimals": 2, "unit_of_measurement": "kg", "device_class": DEVICE_CLASS_EMPTY},
  "mass_lb": {"measurement_type": 0x07, "accuracy_decimals": 2, "unit_of_measurement": "lb", "device_class": DEVICE_CLASS_EMPTY},
  "dewpoint": {"measurement_type": 0x08, "accuracy_decimals": 2, "unit_of_measurement": "°C", "device_class": DEVICE_CLASS_EMPTY},
  "count_small": {"measurement_type": 0x09, "accuracy_decimals": 0, "unit_of_measurement": "", "device_class": DEVICE_CLASS_EMPTY},
  "energy": {"measurement_type": 0x0A, "accuracy_decimals": 3, "unit_of_measurement": "kWh", "device_class": DEVICE_CLASS_ENERGY},
  "power": {"measurement_type": 0x0B, "accuracy_decimals": 2, "unit_of_measurement": "W", "device_class": DEVICE_CLASS_POWER},
  "voltage": {"measurement_type": 0x0C, "accuracy_decimals": 3, "unit_of_measurement": "V", "device_class": DEVICE_CLASS_VOLTAGE},
  "pm2_5": {"measurement_type": 0x0D, "accuracy_decimals": 0, "unit_of_measurement": "ug/m3", "device_class": DEVICE_CLASS_PM25},
  "pm10": {"measurement_type": 0x0E, "accuracy_decimals": 0, "unit_of_measurement": "ug/m3", "device_class": DEVICE_CLASS_PM10},
  "co2": {"measurement_type": 0x12, "accuracy_decimals": 0, "unit_of_measurement": "ppm", "device_class": DEVICE_CLASS_CARBON_DIOXIDE},
  "tvoc": {"measurement_type": 0x13, "accuracy_decimals": 0, "unit_of_measurement": "ug/m3", "device_class": DEVICE_CLASS_VOLATILE_ORGANIC_COMPOUNDS},
  "moisture": {"measurement_type": 0x14, "accuracy_decimals": 2, "unit_of_measurement": "%", "device_class": DEVICE_CLASS_EMPTY},
  "humidity_coarse": {"measurement_type": 0x2E, "accuracy_decimals": 0, "unit_of_measurement": "%", "device_class": DEVICE_CLASS_HUMIDITY},
  "moisture_coarse": {"measurement_type": 0x2F, "accuracy_decimals": 0, "unit_of_measurement": "%", "device_class": DEVICE_CLASS_EMPTY},
  "count_medium": {"measurement_type": 0x3D, "accuracy_decimals": 0, "unit_of_measurement": "", "device_class": DEVICE_CLASS_EMPTY},
  "count_large": {"measurement_type": 0x3E, "accuracy_decimals": 0, "unit_of_measurement": "", "device_class": DEVICE_CLASS_EMPTY},
  "rotation": {"measurement_type": 0x3F, "accuracy_decimals": 1, "unit_of_measurement": "°", "device_class": DEVICE_CLASS_EMPTY},
  "distance_mm": {"measurement_type": 0x40, "accuracy_decimals": 0, "unit_of_measurement": "mm", "device_class": DEVICE_CLASS_EMPTY},
  "distance_m": {"measurement_type": 0x41, "accuracy_decimals": 1, "unit_of_measurement": "m", "device_class": DEVICE_CLASS_EMPTY},
  "duration": {"measurement_type": 0x42, "accuracy_decimals": 3, "unit_of_measurement": "s", "device_class": DEVICE_CLASS_EMPTY},
  "current": {"measurement_type": 0x43, "accuracy_decimals": 3, "unit_of_measurement": "A", "device_class": DEVICE_CLASS_CURRENT},
  "speed": {"measurement_type": 0x44, "accuracy_decimals": 2, "unit_of_measurement": "m/s", "device_class": DEVICE_CLASS_EMPTY},
  "temperature_coarse": {"measurement_type": 0x45, "accuracy_decimals": 1, "unit_of_measurement": "°C", "device_class": DEVICE_CLASS_TEMPERATURE},
  "UV_index": {"measurement_type": 0x46, "accuracy_decimals": 1, "unit_of_measurement": "", "device_class": DEVICE_CLASS_EMPTY},
  "volume": {"measurement_type": 0x47, "accuracy_decimals": 1, "unit_of_measurement": "L", "device_class": DEVICE_CLASS_EMPTY},
  "volume_course": {"measurement_type": 0x48, "accuracy_decimals": 0, "unit_of_measurement": "mL", "device_class": DEVICE_CLASS_EMPTY},
  "volume_Flow_Rate": {"measurement_type": 0x49, "accuracy_decimals": 3, "unit_of_measurement": "m3/hr", "device_class": DEVICE_CLASS_EMPTY},
  "voltage_coarse": {"measurement_type": 0x4A, "accuracy_decimals": 1, "unit_of_measurement": "V", "device_class": DEVICE_CLASS_EMPTY},
  "gas": {"measurement_type": 0x4B, "accuracy_decimals": 3, "unit_of_measurement": "m3", "device_class": DEVICE_CLASS_GAS},
  "gas_large": {"measurement_type": 0x4C, "accuracy_decimals": 3, "unit_of_measurement": "m3", "device_class": DEVICE_CLASS_GAS},
  "energy_coarse": {"measurement_type": 0x4D, "accuracy_decimals": 3, "unit_of_measurement": "kWh", "device_class": DEVICE_CLASS_ENERGY},
  "volume_precise": {"measurement_type": 0x4E, "accuracy_decimals": 3, "unit_of_measurement": "L", "device_class": DEVICE_CLASS_EMPTY},
  "water": {"measurement_type": 0x4F, "accuracy_decimals": 3, "unit_of_measurement": "L", "device_class": DEVICE_CLASS_EMPTY},
}

def _check_measurement_type(value):
    if isinstance(value, int):
        return value
    try:
        return int(value)
    except ValueError:
        pass
      
    if not value in MEASUREMENT_TYPES_SENSOR:
      raise cv.Invalid(F"Invalid measurement type '{value}'!")
    
    return MEASUREMENT_TYPES_SENSOR[value]

def validate_measurement_type(value):
  value = _check_measurement_type(value)
  return value
  
BTHomeDevice = bthome_ns.class_("BTHomeDevice", cg.Component)
BTHomeSensor = bthome_ns.class_("BTHomeSensor", sensor.Sensor, cg.Component)

CONFIG_SCHEMA = cv.All(
  cv.Schema(
    {
      cv.GenerateID(CONF_BTHome_ID): cv.use_id(BTHome),
      cv.GenerateID(): cv.declare_id(BTHomeDevice),
      cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
      cv.Optional(CONF_NAME_PREFIX): cv.string,
      cv.Optional(CONF_DUMP_OPTION): cv.enum(
          DUMP_OPTION, upper=True, space="_"
      ),
      cv.Required(CONF_SENSORS): cv.All( 
        cv.ensure_list(
          sensor.SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(BTHomeSensor),
                cv.Required(CONF_MEASUREMENT_TYPE): validate_measurement_type,
            })
          .extend(cv.COMPONENT_SCHEMA)
          ), 
        cv.Length(min=1)),
      }).extend(cv.COMPONENT_SCHEMA)
)
  
async def to_code(config):
    bthome_component = await cg.get_variable(config[CONF_BTHome_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_address(config[CONF_MAC_ADDRESS].as_hex))
    if CONF_DUMP_OPTION in config:
      cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))

    # iterate around the subsensors
    for i, config_item in enumerate(config[CONF_SENSORS]):
      var_item = cg.new_Pvariable(config_item[CONF_ID])
      if CONF_NAME_PREFIX in config:
        config_item[CONF_NAME] = config[CONF_NAME_PREFIX] + " " + config_item[CONF_NAME]
              
      await cg.register_component(var_item, config_item)
      await sensor.register_sensor(var_item, config_item)
      cg.add(bthome_component.register_sensor(var, config[CONF_MAC_ADDRESS].as_hex, var_item))

      if (isinstance(config_item[CONF_MEASUREMENT_TYPE], dict)):
        measurement_type_record = config_item[CONF_MEASUREMENT_TYPE]
        cg.add(var_item.set_measurement_type(measurement_type_record["measurement_type"]))
        if measurement_type_record["accuracy_decimals"]:
          cg.add(var_item.set_accuracy_decimals(measurement_type_record["accuracy_decimals"]))
        if measurement_type_record["unit_of_measurement"]:
          cg.add(var_item.set_unit_of_measurement(measurement_type_record["unit_of_measurement"]))
        if measurement_type_record["device_class"]:
          cg.add(var_item.set_device_class(measurement_type_record["device_class"]))
      else:
        cg.add(var_item.set_measurement_type(config_item[CONF_MEASUREMENT_TYPE]))
    
