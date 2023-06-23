"""
 Beethowen
 BTHome over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 """

import esphome.codegen as cg
from esphome.components import bthome_receiver_base

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
beethowen_receiver_ns = cg.esphome_ns.namespace("beethowen_receiver")


class Generator(bthome_receiver_base.Generator):
    def hub_factory(self):
        return beethowen_receiver_ns.class_("BeethowenReceiverHub", cg.Component)


gen = Generator()
CONFIG_SCHEMA, to_code = gen.generate_component_config()


def generate_sensor_configs(is_binary_sensor):
    return gen.generate_sensor_configs(is_binary_sensor)
