"""
 Beethowen
 BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 """

import esphome.codegen as cg
from esphome.components import bthome_receiver_base
import esphome.config_validation as cv
from esphome.core import CORE, HexInt, coroutine_with_priority

CODEOWNERS = ["@afarago"]
DEPENDENCIES = []
AUTO_LOAD = [
    "bthome_base",
    "bthome_receiver_base",
    "beethowen_base",
    "binary_sensor",
    "sensor",
]

CONF_BeethowenReceiverHub_ID = "BeethowenReceiverHub_ID"
CONF_LOCAL_PASSKEY = "local_passkey"
CONF_EXPECTED_REMOTE_PASSKEY = "expected_remote_passkey"

beethowen_receiver_ns = cg.esphome_ns.namespace("beethowen_receiver")

BeethowenReceiverDevice = beethowen_receiver_ns.class_(
    "BeethowenReceiverDevice", cg.Component
)


class Generator(bthome_receiver_base.Generator):
    def hub_factory(self):
        return beethowen_receiver_ns.class_("BeethowenReceiverHub", cg.Component)

    def device_class_factory(self):
        return BeethowenReceiverDevice

    def generate_component_schema(self):
        return (
            super()
            .generate_component_schema()
            .extend(
                cv.Schema(
                    {
                        cv.Optional(CONF_LOCAL_PASSKEY): cv.hex_uint16_t,
                    }
                )
            )
        )

    def generate_component_device_schema(
        self,
    ):
        return (
            super()
            .generate_component_device_schema()
            .extend(
                cv.Schema(
                    {
                        cv.Optional(CONF_EXPECTED_REMOTE_PASSKEY): cv.hex_uint16_t,
                    }
                )
            )
        )

    async def to_code(self, config):
        var = await super().to_code(config)
        if CONF_LOCAL_PASSKEY in config:
            cg.add(var.set_local_passkey(HexInt(config[CONF_LOCAL_PASSKEY])))
        return var

    def to_code_device(self, parent, config, ID_PROP):
        devs = super().to_code_device(parent, config, ID_PROP)
        var = devs.device_

        if CONF_EXPECTED_REMOTE_PASSKEY in config:
            cg.add(
                var.set_remote_expected_passkey(
                    HexInt(config[CONF_EXPECTED_REMOTE_PASSKEY])
                )
            )

        return devs


gen = Generator(CONF_BeethowenReceiverHub_ID)
CONFIG_SCHEMA, to_code = gen.generate_component_config()


def generate_sensor_configs(is_binary_sensor):
    return gen.generate_sensor_configs(is_binary_sensor)
