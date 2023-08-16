"""
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 """

import logging
from esphome.cpp_generator import RawExpression, Expression, SafeExpType, safe_exp
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range, has_at_least_one_key
from esphome import automation
from esphome.components import binary_sensor, sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_MAC_ADDRESS,
    CONF_STATE_CLASS,
    CONF_TRIGGER_ID,
    CONF_ACCURACY_DECIMALS,
    CONF_UNIT_OF_MEASUREMENT,
    CONF_DEVICE_CLASS,
    CONF_ICON,
)
from esphome.core import CORE, HexInt, coroutine_with_priority
from esphome.components.bthome_base.const import (
    MEASUREMENT_TYPES_NUMERIC_SENSOR,
    MEASUREMENT_TYPES_BINARY_SENSOR,
)

CONF_BTHomeReceiverBaseDevice_ID = "BTHomeReceiverBaseDevice_ID"
CONF_NAME_PREFIX = "name_prefix"
CONF_DUMP_OPTION = "dump"
CONF_DUMP_PACKETS_OPTION = "dump_packets"
CONF_DEVICES = "devices"
CONF_SENSORS = "sensors"
CONF_MEASUREMENT_TYPE = "measurement_type"
CONF_ON_PACKET = "on_packet"
CONF_ON_EVENT = "on_event"

CODEOWNERS = ["@afarago"]
DEPENDENCIES = []
AUTO_LOAD = [
    "bthome_base",
    "binary_sensor",
    "sensor",
]

bthome_base_ns = cg.global_ns.namespace("bthome_base")
bthome_receiver_base_ns = cg.esphome_ns.namespace("bthome_receiver_base")
BTHomeReceiverBaseDevice = bthome_receiver_base_ns.class_(
    "BTHomeReceiverBaseDevice", cg.Component
)
PacketTrigger = bthome_receiver_base_ns.class_(
    "PacketTrigger", automation.Trigger.template()
)
EventTrigger = bthome_receiver_base_ns.class_(
    "EventTrigger", automation.Trigger.template()
)
BTHomeMeasurementRecord = bthome_base_ns.struct("bthome_measurement_record_t")
BTHomeMeasurementEventRecord = bthome_base_ns.struct(
    "bthome_measurement_event_record_t")

DumpOption = bthome_receiver_base_ns.enum("DumpOption")
DUMP_OPTION = {
    "NONE": DumpOption.DumpOption_None,
    "UNMATCHED": DumpOption.DumpOption_Unmatched,
    "ALL": DumpOption.DumpOption_All,
}

_LOGGER = logging.getLogger(__name__)


class ExplicitClassPtrCast(Expression):
    __slots__ = ("classop", "xhs")

    def __init__(self, classop: SafeExpType, xhs: SafeExpType):
        self.classop = safe_exp(classop).operator("ptr")
        self.xhs = safe_exp(xhs)

    def __str__(self):
        # Surround with parentheses to ensure generated code has same
        # order as python one
        return f"({self.classop})({self.xhs})"


class DeviceStorage:
    device_ = {}
    mac_address_ = {}
    name_prefix_ = {}

    def __init__(self, device_, mac_address_, name_prefix_):
        self.device_ = device_
        self.mac_address_ = mac_address_
        self.name_prefix_ = name_prefix_

    def get_device(self):
        return self.device_

    def get_mac_address(self):
        return self.mac_address_

    def get_name_prefix(self):
        return self.name_prefix_


class Generator:
    hub_ = {}
    hubid_ = {}
    devices_by_addr_ = dict()  # dict of DeviceStorage

    def __init__(self, hubid):
        self.hubid_ = hubid

    def hub_factory(self):
        return bthome_receiver_base_ns.class_("BTHomeReceiverBaseHub", cg.Component)

    def device_class_factory(self):
        return BTHomeReceiverBaseDevice

    def get_hub(self):
        if not self.hub_:
            self.hub_ = self.hub_factory()
        return self.hub_

    def generate_component_config(self):
        CONFIG_SCHEMA = self.generate_component_schema()

        return CONFIG_SCHEMA, self.to_code

    def generate_component_device_schema(self):
        return cv.Schema(
            {
                cv.GenerateID(): cv.declare_id(self.device_class_factory()),
                cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
                cv.Optional(CONF_NAME_PREFIX): cv.string,
                cv.Optional(CONF_DUMP_OPTION): cv.enum(
                    DUMP_OPTION, upper=True, space="_"
                ),
            }
        )

    def generate_component_schema(self):
        CONFIG_SCHEMA = cv.Schema(
            {
                cv.GenerateID(): cv.declare_id(self.get_hub()),
                cv.Optional(CONF_DUMP_OPTION): cv.enum(
                    DUMP_OPTION, upper=True, space="_"
                ),
                cv.Optional(CONF_DUMP_PACKETS_OPTION): cv.boolean,
                cv.Optional(CONF_DEVICES): cv.ensure_list(
                    self.generate_component_device_schema()
                ),
                cv.Optional(CONF_ON_PACKET): automation.validate_automation(
                    {
                        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PacketTrigger),
                    }
                ),
                cv.Optional(CONF_ON_EVENT): automation.validate_automation(
                    {
                        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(EventTrigger),
                    }
                ),
            }
        ).extend(cv.COMPONENT_SCHEMA)
        return CONFIG_SCHEMA

    async def to_code_device(self, parent, config, ID_PROP):
        # add to device registry
        mac_address_str = str(config[CONF_MAC_ADDRESS])
        if not mac_address_str in self.devices_by_addr_:
            var = cg.Pvariable(
                config[ID_PROP],
                ExplicitClassPtrCast(
                    self.device_class_factory(),
                    parent.add_device(config[CONF_MAC_ADDRESS].as_hex),
                ),
            )
            await cg.register_component(var, config)

            name_prefix_str = (
                str(config[CONF_NAME_PREFIX]
                    ) if CONF_NAME_PREFIX in config else None
            )
            devs = DeviceStorage(var, mac_address_str, name_prefix_str)
            self.devices_by_addr_[mac_address_str] = devs

        else:
            devs = self.devices_by_addr_[mac_address_str]
            var = devs.get_device()

        if CONF_NAME_PREFIX in config:
            cg.add(var.set_name_prefix(config[CONF_NAME_PREFIX]))
        if CONF_DUMP_OPTION in config:
            cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))

        return devs

    async def to_code(self, config):
        var = cg.new_Pvariable(config[CONF_ID])
        await cg.register_component(var, config)

        if CONF_DUMP_OPTION in config:
            cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))
        if CONF_DUMP_PACKETS_OPTION in config:
            cg.add(var.set_dump_packets_option(
                config[CONF_DUMP_PACKETS_OPTION]))

        # iterate around the devices
        if CONF_DEVICES in config:
            for i, config_item in enumerate(config[CONF_DEVICES]):
                await self.to_code_device(var, config_item, CONF_ID)

        # automations
        for conf in config.get(CONF_ON_PACKET, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(
                trigger,
                [
                    (cg.uint64.operator("const"), "address"),
                    (
                        cg.std_vector.template(BTHomeMeasurementRecord).operator(
                            "const"
                        ),
                        "measurements",
                    ),
                ],
                conf,
            )
        for conf in config.get(CONF_ON_EVENT, []):
            trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
            await automation.build_automation(
                trigger,
                [
                    (cg.uint64.operator("const"), "address"),
                    (BTHomeMeasurementEventRecord.operator("const"), "event")
                ],
                conf,
            )

        return var

    def sensor_schema_subsensor_factory(
        self, schema_base, ReceiverSensor, validate_measurement_type
    ):
        return schema_base.extend(
            {
                cv.GenerateID(): cv.declare_id(ReceiverSensor),
                cv.Required(CONF_MEASUREMENT_TYPE): validate_measurement_type,
            }
        )

    def sensor_schema_factory(
        self, schema_base, ReceiverSensor, validate_measurement_type
    ):
        CONFIG_SCHEMA = cv.Schema(
            {
                cv.GenerateID(self.hubid_): cv.use_id(self.get_hub()),
                cv.GenerateID(): cv.declare_id(ReceiverSensor),
                cv.GenerateID(CONF_BTHomeReceiverBaseDevice_ID): cv.declare_id(
                    self.device_class_factory()
                ),
                cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
                cv.Required(CONF_SENSORS): cv.All(
                    cv.ensure_list(
                        self.sensor_schema_subsensor_factory(
                            schema_base, ReceiverSensor, validate_measurement_type
                        )
                    ),
                    cv.Length(min=1),
                ),
            }
        ).extend(cv.COMPONENT_SCHEMA)
        return CONFIG_SCHEMA

    def generate_sensor_configs(self, is_binary_sensor):
        sensor_base = binary_sensor.BinarySensor if is_binary_sensor else sensor.Sensor
        MEASUREMENT_TYPES = (
            MEASUREMENT_TYPES_BINARY_SENSOR
            if is_binary_sensor
            else MEASUREMENT_TYPES_NUMERIC_SENSOR
        )
        schema_base = (
            binary_sensor.binary_sensor_schema()
            if is_binary_sensor
            else sensor.sensor_schema()
        )
        register_sensor_async_fn = (
            binary_sensor.register_binary_sensor
            if is_binary_sensor
            else sensor.register_sensor
        )
        cpp_classname = (
            "BTHomeReceiverBaseBinarySensor"
            if is_binary_sensor
            else "BTHomeReceiverBaseSensor"
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

        def _get_measurement_type_value(confitem):
            value2 = confitem[CONF_MEASUREMENT_TYPE]
            if isinstance(value2, dict):
                value2 = value2[CONF_MEASUREMENT_TYPE]
            else:
                value2 = int(value2)
            return value2

        ReceiverSensor = bthome_receiver_base_ns.class_(
            cpp_classname, sensor_base, cg.Component
        )

        CONFIG_SCHEMA = self.sensor_schema_factory(
            schema_base, ReceiverSensor, validate_measurement_type
        )

        async def to_code(config):
            paren = await cg.get_variable(config[self.hubid_])

            mac_address = config[CONF_MAC_ADDRESS]
            devs = await self.to_code_device(paren, config, CONF_BTHomeReceiverBaseDevice_ID)

            # check if sensors are in order, rearrange them if needed
            config_sensors_arr = config[CONF_SENSORS]
            if not all(
                _get_measurement_type_value(config_sensors_arr[i])
                <= _get_measurement_type_value(config_sensors_arr[i + 1])
                for i in range(len(config_sensors_arr) - 1)
            ):
                _LOGGER.warning(
                    "Warning: Receiver sensors array not in increasing order, rearranging automatically. Consder rearranging them in config manually."
                )
                # bubble sort sensors, stable sort, order on same ones are kept
                swapped = True
                for i in range(len(config_sensors_arr) - 1):
                    # Last i elements are already in place
                    for j in range(0, len(config_sensors_arr) - 1 - i):
                        if _get_measurement_type_value(
                            config_sensors_arr[j]
                        ) > _get_measurement_type_value(config_sensors_arr[j + 1]):
                            swapped = True
                            config_sensors_arr[j], config_sensors_arr[j + 1] = (
                                config_sensors_arr[j + 1],
                                config_sensors_arr[j],
                            )
                    if not swapped:
                        break

            # iterate around the subsensors
            for i, config_item in enumerate(config_sensors_arr):
                var_item = cg.new_Pvariable(config_item[CONF_ID])

                if devs.get_name_prefix():
                    config_item[CONF_NAME] = (
                        devs.get_name_prefix() + " " + config_item[CONF_NAME]
                    )

                await register_sensor_async_fn(var_item, config_item)

                if isinstance(config_item[CONF_MEASUREMENT_TYPE], dict):
                    measurement_type_record = config_item[CONF_MEASUREMENT_TYPE]

                    cg.add(
                        var_item.set_measurement_type(
                            HexInt(
                                measurement_type_record[CONF_MEASUREMENT_TYPE])
                        )
                    )

                    if (
                        measurement_type_record.get(CONF_ACCURACY_DECIMALS)
                        and not CONF_ACCURACY_DECIMALS in config_item
                    ):
                        cg.add(
                            var_item.set_accuracy_decimals(
                                measurement_type_record[CONF_ACCURACY_DECIMALS]
                            )
                        )
                    if (
                        measurement_type_record.get(CONF_UNIT_OF_MEASUREMENT)
                        and not CONF_UNIT_OF_MEASUREMENT in config_item
                    ):
                        cg.add(
                            var_item.set_unit_of_measurement(
                                measurement_type_record[CONF_UNIT_OF_MEASUREMENT]
                            )
                        )
                    if (
                        measurement_type_record.get(CONF_DEVICE_CLASS)
                        and not CONF_DEVICE_CLASS in config_item
                    ):
                        cg.add(
                            var_item.set_device_class(
                                measurement_type_record[CONF_DEVICE_CLASS]
                            )
                        )
                    if (
                        measurement_type_record.get(CONF_ICON)
                        and not CONF_ICON in config_item
                    ):
                        cg.add(var_item.set_icon(
                            measurement_type_record[CONF_ICON]))
                else:
                    cg.add(
                        var_item.set_measurement_type(
                            config_item[CONF_MEASUREMENT_TYPE]
                        )
                    )

                # last statement - add the sensor
                cg.add(
                    paren.add_sensor(
                        devs.get_device(
                        ), config[CONF_MAC_ADDRESS].as_hex, var_item
                    )
                )

        return CONFIG_SCHEMA, to_code
