import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import esp32_ble_tracker
from esphome.const import CONF_ID
from esphome.core import CORE

CODEOWNERS = ["@havanti"]
DEPENDENCIES = ["esp32_ble_tracker"]

victron_scanner_ns = cg.esphome_ns.namespace("victron_scanner")
VictronListener = victron_scanner_ns.class_(
    "VictronListener", esp32_ble_tracker.ESPBTDeviceListener
)


def _validate_esp_idf(config):
    if not (CORE.is_esp32 and not CORE.using_arduino):
        raise cv.Invalid("victron_scanner requires the ESP32 platform with the ESP-IDF framework")
    return config


CONFIG_SCHEMA = cv.All(
    _validate_esp_idf,
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(VictronListener),
        }
    ).extend(esp32_ble_tracker.ESP_BLE_DEVICE_SCHEMA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await esp32_ble_tracker.register_ble_device(var, config)
