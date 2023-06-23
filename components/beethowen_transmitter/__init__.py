"""
 Beethowen
 BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 """

from esphome.cpp_generator import RawExpression
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range, has_at_least_one_key
from esphome import automation
from esphome.components import template, binary_sensor, sensor, esp32_ble_tracker
from esphome.components.template import sensor as template_sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_LAMBDA,
)
from esphome.core import CORE, coroutine_with_priority

CONF_Beethowen_ID = "Beethowen_ID"
CONF_MEASUREMENT_TYPE = "measurement_type"
CONF_CONNECT_PERSISTENT = "connect_persistent"

CODEOWNERS = ["@afarago"]
DEPENDENCIES = []
AUTO_LOAD = ["beethowen_base", "binary_sensor", "sensor", "template"]

beethowen_transmitter_ns = cg.esphome_ns.namespace("beethowen_transmitter")
BeethowenTransmitterHub = beethowen_transmitter_ns.class_(
    "BeethowenTransmitterHub", cg.Component
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BeethowenTransmitterHub),
            cv.Optional(CONF_CONNECT_PERSISTENT): cv.boolean,
        }
    ).extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_CONNECT_PERSISTENT in config:
        cg.add(var.set_connect_persistent(config[CONF_CONNECT_PERSISTENT]))


# shared sensor configs
def beethowen_shared_sensor_configs(is_binary_sensor, MEASUREMENT_TYPES):
    sensor_base = binary_sensor.BinarySensor if is_binary_sensor else sensor.Sensor
    schema_base = (
        binary_sensor.BINARY_SENSOR_SCHEMA
        if is_binary_sensor
        else template_sensor.CONFIG_SCHEMA
    )
    register_async_fn = (
        binary_sensor.register_binary_sensor
        if is_binary_sensor
        else sensor.register_sensor
    )
    cpp_classname = (
        "BeethowenTransmitterBinarySensor"
        if is_binary_sensor
        else "BeethowenTransmitterSensor"
    )

    def _check_measurement_type(value):
        if isinstance(value, int):
            return value
        try:
            return int(value)
        except ValueError:
            pass

        if not value in MEASUREMENT_TYPES:
            raise cv.Invalid(f"Invalid measurement type '{value}'!")

        return MEASUREMENT_TYPES[value]

    def validate_measurement_type(value):
        value = _check_measurement_type(value)
        return value

    BeethowenTransmitterSensor = beethowen_transmitter_ns.class_(
        cpp_classname, sensor_base, cg.Component
    )

    CONFIG_SCHEMA = cv.All(
        schema_base.extend(
            {
                cv.GenerateID(CONF_Beethowen_ID): cv.use_id(BeethowenTransmitterHub),
                cv.GenerateID(): cv.declare_id(BeethowenTransmitterSensor),
                cv.Required(CONF_MEASUREMENT_TYPE): validate_measurement_type,
            }
        ).extend(cv.COMPONENT_SCHEMA)
    )

    async def to_code(config):
        paren = await cg.get_variable(config[CONF_Beethowen_ID])
        var = cg.new_Pvariable(config[CONF_ID])
        await cg.register_component(var, config)

        await register_async_fn(var, config)
        cg.add(paren.register_sensor(var))

        if isinstance(config[CONF_MEASUREMENT_TYPE], dict):
            measurement_type_record = config[CONF_MEASUREMENT_TYPE]

            cg.add(
                var.set_measurement_type(measurement_type_record["measurement_type"])
            )
            if (
                measurement_type_record.get("accuracy_decimals")
                and not "accuracy_decimals" in config
            ):
                cg.add(
                    var.set_accuracy_decimals(
                        measurement_type_record["accuracy_decimals"]
                    )
                )
            if (
                measurement_type_record.get("unit_of_measurement")
                and not "unit_of_measurement" in config
            ):
                cg.add(
                    var.set_unit_of_measurement(
                        measurement_type_record["unit_of_measurement"]
                    )
                )
            if (
                measurement_type_record.get("device_class")
                and not "device_class" in config
            ):
                cg.add(var.set_device_class(measurement_type_record["device_class"]))
        else:
            cg.add(var.set_measurement_type(config[CONF_MEASUREMENT_TYPE]))

        if CONF_LAMBDA in config:
            template_ = await cg.process_lambda(
                config[CONF_LAMBDA], [], return_type=cg.optional.template(float)
            )
            cg.add(var.set_template(template_))

    return CONFIG_SCHEMA, to_code
