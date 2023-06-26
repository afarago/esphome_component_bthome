"""
 BTHome protocol virtual sensors for ESPHome

 Author: Attila Farago
 """

import esphome.codegen as cg
from esphome.components import bthome_receiver_base, esp32_ble_tracker

CODEOWNERS = ["@afarago"]
DEPENDENCIES = ["esp32_ble_tracker"]
AUTO_LOAD = [
    "bthome_base",
    "bthome_receiver_base",
    "binary_sensor",
    "sensor",
]

CONF_BTHomeReceiverHub_ID = "BTHomeReceiverHub_ID"
bthome_receiver_ns = cg.esphome_ns.namespace("bthome_receiver")


class Generator(bthome_receiver_base.Generator):
    def hub_factory(self):
        return bthome_receiver_ns.class_(
            "BTHomeReceiverHub", esp32_ble_tracker.ESPBTDeviceListener, cg.Component
        )

    def generate_component_schema(self):
        return (
            super()
            .generate_component_schema()
            .extend(esp32_ble_tracker.ESP_BLE_DEVICE_SCHEMA)
        )
    
    async def generate_to_code_body(self, config):
        var = await super().generate_to_code_body(config)
        await esp32_ble_tracker.register_ble_device(var, config)
        
        return var

gen = Generator(CONF_BTHomeReceiverHub_ID)
CONFIG_SCHEMA, to_code = gen.generate_component_config()

def generate_sensor_configs(is_binary_sensor):
    return gen.generate_sensor_configs(is_binary_sensor)
