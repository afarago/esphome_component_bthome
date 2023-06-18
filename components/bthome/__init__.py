from esphome.cpp_generator import RawExpression
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range, has_at_least_one_key
from esphome import automation
from esphome.components import binary_sensor, sensor, esp32_ble_tracker
from esphome.const import (
  CONF_ID,
  CONF_NAME,
  CONF_MAC_ADDRESS,
)
from esphome.core import CORE, coroutine_with_priority

CONF_BTHome_ID = "BTHome_ID"
CONF_DUMP_OPTION = "dump"

CONF_NAME_PREFIX = "name_prefix"
CONF_SENSORS = "sensors"
CONF_MEASUREMENT_TYPE = "measurement_type"

CODEOWNERS = ["@afarago"]
DEPENDENCIES = ["esp32_ble_tracker"]
AUTO_LOAD = ["binary_sensor", "sensor"]

# CONF_ON_CODE_RECEIVED = "on_code_received"
# hex_uint20_t = hex_int_range(min=0, max=1048575)

bthome_ns = cg.esphome_ns.namespace("bthome")
BTHome = bthome_ns.class_("BTHome", esp32_ble_tracker.ESPBTDeviceListener, cg.Component)

DumpOption = bthome_ns.enum("DumpOption")
DUMP_OPTION = {
    "NONE": DumpOption.DumpOption_None,
    "UNMATCHED": DumpOption.DumpOption_Unmatched,
    "ALL": DumpOption.DumpOption_All,
}

CONFIG_SCHEMA = cv.All(
        cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BTHome),
            cv.Optional(CONF_DUMP_OPTION): cv.enum(
                DUMP_OPTION, upper=True, space="_"
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(esp32_ble_tracker.ESP_BLE_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await esp32_ble_tracker.register_ble_device(var, config)

    if CONF_DUMP_OPTION in config:
      cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))
      
# shared sensor configs
def bthome_shared_sensor_configs(is_binary_sensor, MEASUREMENT_TYPES):
  
  sensor_base = binary_sensor.BinarySensor if is_binary_sensor else sensor.Sensor
  schema_base = binary_sensor.BINARY_SENSOR_SCHEMA if is_binary_sensor else sensor.SENSOR_SCHEMA
  register_async_fn = binary_sensor.register_binary_sensor if is_binary_sensor else sensor.register_sensor
  cpp_classname = "BTHomeBinarySensor" if is_binary_sensor else "BTHomeSensor"

  BTHomeDevice = bthome_ns.class_("BTHomeDevice", cg.Component)

  def _check_measurement_type(value):
      if isinstance(value, int):
          return value
      try:
          return int(value)
      except ValueError:
          pass
        
      if not value in MEASUREMENT_TYPES:
        raise cv.Invalid(F"Invalid measurement type '{value}'!")
      
      return MEASUREMENT_TYPES[value]

  def validate_measurement_type(value):
    value = _check_measurement_type(value)
    return value

  BTHomeSensor = bthome_ns.class_(cpp_classname, sensor_base, cg.Component)

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
            schema_base.extend(
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
        await register_async_fn(var_item, config_item)
        cg.add(bthome_component.register_sensor(var, config[CONF_MAC_ADDRESS].as_hex, var_item))

        if (isinstance(config_item[CONF_MEASUREMENT_TYPE], dict)):
          measurement_type_record = config_item[CONF_MEASUREMENT_TYPE]
          
          cg.add(var_item.set_measurement_type(measurement_type_record["measurement_type"]))
          if measurement_type_record.get("accuracy_decimals"):
            cg.add(var_item.set_accuracy_decimals(measurement_type_record["accuracy_decimals"]))
          if measurement_type_record.get("unit_of_measurement"):
            cg.add(var_item.set_unit_of_measurement(measurement_type_record["unit_of_measurement"]))
          if measurement_type_record.get("device_class"):
            cg.add(var_item.set_device_class(measurement_type_record["device_class"]))
        else:
          cg.add(var_item.set_measurement_type(config_item[CONF_MEASUREMENT_TYPE]))
          
  return CONFIG_SCHEMA, to_code
