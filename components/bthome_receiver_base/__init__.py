"""
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 """

from esphome.cpp_generator import RawExpression
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range, has_at_least_one_key
from esphome import automation
from esphome.components import binary_sensor, sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_MAC_ADDRESS,
)
from esphome.core import CORE, HexInt, coroutine_with_priority
from esphome.components.bthome_base.const import (
    MEASUREMENT_TYPES_NUMERIC_SENSOR,
    MEASUREMENT_TYPES_BINARY_SENSOR,
)

CONF_BTHomeReceiverBaseDevice_ID = "BTHomeReceiverBaseDevice_ID"
CONF_NAME_PREFIX = "name_prefix"
CONF_DUMP_OPTION = "dump"
CONF_DEVICES = "devices"
CONF_SENSORS = "sensors"
CONF_MEASUREMENT_TYPE = "measurement_type"

CODEOWNERS = ["@afarago"]
DEPENDENCIES = []
AUTO_LOAD = [
    "bthome_base",
    "binary_sensor",
    "sensor",
]

bthome_receiver_base_ns = cg.esphome_ns.namespace("bthome_receiver_base")
BTHomeReceiverBaseDevice = bthome_receiver_base_ns.class_(
    "BTHomeReceiverBaseDevice", cg.Component
)

DumpOption = bthome_receiver_base_ns.enum("DumpOption")
DUMP_OPTION = {
    "NONE": DumpOption.DumpOption_None,
    "UNMATCHED": DumpOption.DumpOption_Unmatched,
    "ALL": DumpOption.DumpOption_All,
}


class DeviceStorage:
    device_ = {}
    mac_address_ = {}
    name_prefix_ = {}

    def __init__(self, device_, mac_address_, name_prefix_):
        self.device_ = device_
        self.mac_address_ = mac_address_
        self.name_prefix_ = name_prefix_


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
                cv.Optional(CONF_DEVICES): cv.ensure_list(
                    self.generate_component_device_schema()
                ),
            }
        ).extend(cv.COMPONENT_SCHEMA)
        return CONFIG_SCHEMA

    def to_code_device(self, parent, config, ID_PROP):
        var = cg.new_Pvariable(config[ID_PROP])
        cg.add(var.set_address(config[CONF_MAC_ADDRESS].as_hex))
        name_prefix_str = None
        if CONF_NAME_PREFIX in config:
            cg.add(var.set_name_prefix(config[CONF_NAME_PREFIX]))
            name_prefix_str = str(config[CONF_NAME_PREFIX])
        if CONF_DUMP_OPTION in config:
            cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))

        mac_address_str = str(config[CONF_MAC_ADDRESS])
        if mac_address_str in self.devices_by_addr_:
            raise cv.Invalid("Device already registered.")

        devs = DeviceStorage(var, mac_address_str, name_prefix_str)
        self.devices_by_addr_[mac_address_str] = devs
        cg.add(parent.register_device(config[CONF_MAC_ADDRESS].as_hex, var))

        return devs

    async def to_code(self, config):
        var = cg.new_Pvariable(config[CONF_ID])
        await cg.register_component(var, config)

        if CONF_DUMP_OPTION in config:
            cg.add(var.set_dump_option(config[CONF_DUMP_OPTION]))

        # iterate around the devices
        if CONF_DEVICES in config:
            for i, config_item in enumerate(config[CONF_DEVICES]):
                self.to_code_device(var, config_item, CONF_ID)

        return var

    def sensor_schema_subsensor_factory(
        self, schema_base, ReceiverSensor, validate_measurement_type
    ):
        return schema_base.extend(
            {
                cv.GenerateID(): cv.declare_id(ReceiverSensor),
                cv.Required(CONF_MEASUREMENT_TYPE): validate_measurement_type,
            }
        ).extend(cv.COMPONENT_SCHEMA)

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
            binary_sensor.BINARY_SENSOR_SCHEMA
            if is_binary_sensor
            else sensor.SENSOR_SCHEMA
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

        ReceiverSensor = bthome_receiver_base_ns.class_(
            cpp_classname, sensor_base, cg.Component
        )

        CONFIG_SCHEMA = self.sensor_schema_factory(
            schema_base, ReceiverSensor, validate_measurement_type
        )

        async def to_code(config):
            paren = await cg.get_variable(config[self.hubid_])

            mac_address = config[CONF_MAC_ADDRESS]
            devs = None
            if str(mac_address) in self.devices_by_addr_:
                devs = self.devices_by_addr_[str(mac_address)]
            else:
                devs = self.to_code_device(
                    paren, config, CONF_BTHomeReceiverBaseDevice_ID
                )

            # iterate around the subsensors
            for i, config_item in enumerate(config[CONF_SENSORS]):
                var_item = cg.new_Pvariable(config_item[CONF_ID])

                if devs.name_prefix_:
                    config_item[CONF_NAME] = (
                        devs.name_prefix_ + " " + config_item[CONF_NAME]
                    )

                await cg.register_component(var_item, config_item)
                await register_sensor_async_fn(var_item, config_item)

                if isinstance(config_item[CONF_MEASUREMENT_TYPE], dict):
                    measurement_type_record = config_item[CONF_MEASUREMENT_TYPE]

                    cg.add(
                        var_item.set_measurement_type(
                            HexInt(measurement_type_record["measurement_type"])
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
                        var_item.set_measurement_type(
                            config_item[CONF_MEASUREMENT_TYPE]
                        )
                    )

                # last statement - add the sensor
                cg.add(
                    paren.add_sensor(
                        devs.device_, config[CONF_MAC_ADDRESS].as_hex, var_item
                    )
                )

        return CONFIG_SCHEMA, to_code
