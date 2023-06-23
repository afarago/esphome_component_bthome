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
from esphome.components import binary_sensor, sensor, esp32_ble_tracker
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_MAC_ADDRESS,
)
from esphome.core import CORE, coroutine_with_priority

CONF_Beethowen_ID = "Beethowen_ID"
CONF_NAME_PREFIX = "name_prefix"
CONF_DUMP_OPTION = "dump"
CONF_SENSORS = "sensors"
CONF_MEASUREMENT_TYPE = "measurement_type"

CODEOWNERS = ["@afarago"]
DEPENDENCIES = []
AUTO_LOAD = ["beethowen_base", "binary_sensor", "sensor"]

beethowen_receiver_ns = cg.esphome_ns.namespace("beethowen_receiver")
BeethowenReceiverHub = beethowen_receiver_ns.class_(
    "BeethowenReceiverHub", cg.Component
)

DumpOption = beethowen_receiver_ns.enum("DumpOption")
DUMP_OPTION = {
    "NONE": DumpOption.DumpOption_None,
    "UNMATCHED": DumpOption.DumpOption_Unmatched,
    "ALL": DumpOption.DumpOption_All,
}

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BeethowenReceiverHub),
            cv.Optional(CONF_DUMP_OPTION): cv.enum(DUMP_OPTION, upper=True, space="_"),
        }
    ).extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    # await esp32_ble_tracker.register_ble_device(var, config)

    if CONF_DUMP_OPTION in config:
        cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))


# shared sensor configs
def beethowen_shared_sensor_configs(is_binary_sensor, MEASUREMENT_TYPES):
    sensor_base = binary_sensor.BinarySensor if is_binary_sensor else sensor.Sensor
    schema_base = (
        binary_sensor.BINARY_SENSOR_SCHEMA if is_binary_sensor else sensor.SENSOR_SCHEMA
    )
    register_async_fn = (
        binary_sensor.register_binary_sensor
        if is_binary_sensor
        else sensor.register_sensor
    )
    cpp_classname = (
        "BeethowenReceiverBinarySensor"
        if is_binary_sensor
        else "BeethowenReceiverSensor"
    )

    BeethowenReceiverDevice = beethowen_receiver_ns.class_(
        "BeethowenReceiverDevice", cg.Component
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

    BeethowenReceiverSensor = beethowen_receiver_ns.class_(
        cpp_classname, sensor_base, cg.Component
    )

    CONFIG_SCHEMA = cv.All(
        cv.Schema(
            {
                cv.GenerateID(CONF_Beethowen_ID): cv.use_id(BeethowenReceiverHub),
                cv.GenerateID(): cv.declare_id(BeethowenReceiverDevice),
                cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
                cv.Optional(CONF_NAME_PREFIX): cv.string,
                cv.Optional(CONF_DUMP_OPTION): cv.enum(
                    DUMP_OPTION, upper=True, space="_"
                ),
                cv.Required(CONF_SENSORS): cv.All(
                    cv.ensure_list(
                        schema_base.extend(
                            {
                                cv.GenerateID(): cv.declare_id(BeethowenReceiverSensor),
                                cv.Required(
                                    CONF_MEASUREMENT_TYPE
                                ): validate_measurement_type,
                            }
                        ).extend(cv.COMPONENT_SCHEMA)
                    ),
                    cv.Length(min=1),
                ),
            }
        ).extend(cv.COMPONENT_SCHEMA)
    )

    async def to_code(config):
        paren = await cg.get_variable(config[CONF_Beethowen_ID])
        var = cg.new_Pvariable(config[CONF_ID])
        await cg.register_component(var, config)

        cg.add(var.set_address(config[CONF_MAC_ADDRESS].as_hex))
        cg.add(paren.register_device(var))

        if CONF_DUMP_OPTION in config:
            cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))

        # iterate around the subsensors
        for i, config_item in enumerate(config[CONF_SENSORS]):
            var_item = cg.new_Pvariable(config_item[CONF_ID])
            if CONF_NAME_PREFIX in config:
                config_item[CONF_NAME] = (
                    config[CONF_NAME_PREFIX] + " " + config_item[CONF_NAME]
                )

            await cg.register_component(var_item, config_item)
            await register_async_fn(var_item, config_item)
            cg.add(
                paren.register_sensor(var, config[CONF_MAC_ADDRESS].as_hex, var_item)
            )

            if isinstance(config_item[CONF_MEASUREMENT_TYPE], dict):
                measurement_type_record = config_item[CONF_MEASUREMENT_TYPE]

                cg.add(
                    var_item.set_measurement_type(
                        measurement_type_record["measurement_type"]
                    )
                )
                if (
                    measurement_type_record.get("accuracy_decimals")
                    and not "accuracy_decimals" in config
                ):
                    cg.add(
                        var_item.set_accuracy_decimals(
                            measurement_type_record["accuracy_decimals"]
                        )
                    )
                if (
                    measurement_type_record.get("unit_of_measurement")
                    and not "unit_of_measurement" in config
                ):
                    cg.add(
                        var_item.set_unit_of_measurement(
                            measurement_type_record["unit_of_measurement"]
                        )
                    )
                if (
                    measurement_type_record.get("device_class")
                    and not "device_class" in config
                ):
                    cg.add(
                        var_item.set_device_class(
                            measurement_type_record["device_class"]
                        )
                    )
            else:
                cg.add(
                    var_item.set_measurement_type(config_item[CONF_MEASUREMENT_TYPE])
                )

    return CONFIG_SCHEMA, to_code
