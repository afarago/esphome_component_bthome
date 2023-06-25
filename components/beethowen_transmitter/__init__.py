"""
 Beethowen
 BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 """

from functools import partial
from esphome.cpp_generator import RawExpression
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range, has_at_least_one_key
from esphome import automation
from esphome.components import binary_sensor, sensor
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome.core import CORE, HexInt, coroutine_with_priority
from esphome.components.bthome_base.const import (
    MEASUREMENT_TYPES_SENSOR,
    MEASUREMENT_TYPES_BINARY_SENSOR,
)

CONF_Beethowen_ID = "Beethowen_ID"
CONF_MEASUREMENT_TYPE = "measurement_type"
CONF_CONNECT_PERSISTENT = "connect_persistent"
CONF_SENSORS = "sensors"
CONF_AUTO_SEND = "auto_send"
CONF_SENSOR_SENSOR_ID = "sensor_id"
CONF_ON_FINISHED_SEND = "on_finished_send"

CODEOWNERS = ["@afarago"]
DEPENDENCIES = []
AUTO_LOAD = ["bthome_base", "beethowen_base", "binary_sensor", "sensor"]

beethowen_transmitter_ns = cg.esphome_ns.namespace("beethowen_transmitter")
BeethowenTransmitterHub = beethowen_transmitter_ns.class_(
    "BeethowenTransmitterHub", cg.Component
)
BeethowenTransmitterSensor = beethowen_transmitter_ns.class_(
    "BeethowenTransmitterSensor", cg.Component, sensor.Sensor
)
BeethowenTransmitterBinarySensor = beethowen_transmitter_ns.class_(
    "BeethowenTransmitterBinarySensor", cg.Component, binary_sensor.BinarySensor
)
FinishedSendTrigger = beethowen_transmitter_ns.class_(
    "FinishedSendTrigger ", automation.Trigger.template(bool, bool)
)
# PlayAction = rtttl_ns.class_("PlayAction", automation.Action)


def validate_proxy_id(value):
    value = cv.string_strict(value)
    value = cv.Length(max=20)(value)
    return value


def create_check_measurement_type_fn(measurement_types):
    def validate_measurement_fn(value):
        if isinstance(value, int):
            return value
        try:
            return int(value)
        except ValueError:
            pass

        if not value in measurement_types:
            raise cv.Invalid(f"Invalid measurement type '{value}'!")

        return measurement_types[value]

    return validate_measurement_fn


validate_sensor_measurement_type = create_check_measurement_type_fn(
    MEASUREMENT_TYPES_SENSOR
)
validate_binary_sensor_measurement_type = create_check_measurement_type_fn(
    MEASUREMENT_TYPES_BINARY_SENSOR
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BeethowenTransmitterHub),
            cv.Optional(CONF_CONNECT_PERSISTENT): cv.boolean,
            cv.Optional(CONF_AUTO_SEND): cv.boolean,
            cv.Optional(CONF_ON_FINISHED_SEND): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(FinishedSendTrigger),
                }
            ),
            cv.Required(CONF_SENSORS): cv.All(
                cv.ensure_list(
                    cv.Schema(
                        {
                            cv.GenerateID(): cv.declare_id(BeethowenTransmitterSensor),
                            cv.Required(
                                CONF_MEASUREMENT_TYPE
                            ): validate_sensor_measurement_type,
                            cv.Required(CONF_SENSOR_SENSOR_ID): cv.use_id(
                                sensor.Sensor
                            ),
                        }
                    )
                )
            ),
        }
    ).extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_CONNECT_PERSISTENT in config:
        cg.add(var.set_connect_persistent(config[CONF_CONNECT_PERSISTENT]))

    for config_item in config.get(CONF_SENSORS, []):
        sensor = await cg.get_variable(config_item[CONF_SENSOR_SENSOR_ID])

        measurement_type = config_item[CONF_MEASUREMENT_TYPE]
        if isinstance(measurement_type, dict):
            measurement_type = measurement_type["measurement_type"]

        cg.add(var.add_sensor(HexInt(measurement_type), sensor))

    for conf in config.get(CONF_ON_FINISHED_SEND, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger, [(bool, "success"), (bool, "has_outstanding_measurements")], conf
        )


# @automation.register_action(
#     "rtttl.play",
#     PlayAction,
#     cv.maybe_simple_value(
#         {
#             cv.GenerateID(CONF_ID): cv.use_id(Rtttl),
#             cv.Required(CONF_RTTTL): cv.templatable(cv.string),
#         },
#         key=CONF_RTTTL,
#     ),
# )
# async def rtttl_play_to_code(config, action_id, template_arg, args):
#     paren = await cg.get_variable(config[CONF_ID])
#     var = cg.new_Pvariable(action_id, template_arg, paren)
#     template_ = await cg.templatable(config[CONF_RTTTL], args, cg.std_string)
#     cg.add(var.set_value(template_))
#     return var
