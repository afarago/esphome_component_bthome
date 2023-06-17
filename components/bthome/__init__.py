from esphome.cpp_generator import RawExpression
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.config_validation import hex_int_range, has_at_least_one_key
from esphome import automation
from esphome.components import binary_sensor, sensor, esp32_ble_tracker
from esphome.const import (
    CONF_ID,
)
from esphome.core import CORE, coroutine_with_priority

CONF_BTHome_ID = "BTHome_ID"
CONF_DUMP_UNMATCHED = "dump_unmatched";

CODEOWNERS = ["@afarago"]
DEPENDENCIES = ["esp32_ble_tracker"]
AUTO_LOAD = ["binary_sensor", "sensor"]

# CONF_ON_CODE_RECEIVED = "on_code_received"
# hex_uint20_t = hex_int_range(min=0, max=1048575)

bthome_ns = cg.esphome_ns.namespace("bthome")
BTHome = bthome_ns.class_("BTHome", esp32_ble_tracker.ESPBTDeviceListener, cg.Component)

CONFIG_SCHEMA = cv.All(
        cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BTHome),
            cv.Optional(CONF_DUMP_UNMATCHED): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(esp32_ble_tracker.ESP_BLE_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await esp32_ble_tracker.register_ble_device(var, config)

    if CONF_DUMP_UNMATCHED in config:
      cg.add(var.set_dump_unmatched_packages(config[CONF_DUMP_UNMATCHED]))
    
    