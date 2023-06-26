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
# CONF_LOCAL_PASSKEY = "local_passkey"
# CONF_REMOTE_EXPECTED_PASSKEY = "remote_expected_passkey"

beethowen_receiver_ns = cg.esphome_ns.namespace("beethowen_receiver")


class Generator(bthome_receiver_base.Generator):
    def hub_factory(self):
        return beethowen_receiver_ns.class_("BeethowenReceiverHub", cg.Component)


gen = Generator(CONF_BeethowenReceiverHub_ID)
CONFIG_SCHEMA, to_code = gen.generate_component_config()


def generate_sensor_configs(is_binary_sensor):
    return gen.generate_sensor_configs(is_binary_sensor)


# TODO add to_code, add to SCHEMA
#     if CONF_LOCAL_PASSKEY in config:
#         cg.add(var.set_local_passkey(HexInt(config[CONF_LOCAL_PASSKEY])))
#     if CONF_REMOTE_EXPECTED_PASSKEY in config:
#         cg.add(var.set_remote_expected_passkey(HexInt(config[CONF_REMOTE_EXPECTED_PASSKEY])))
