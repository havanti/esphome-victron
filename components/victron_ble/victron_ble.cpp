#include "victron_ble.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32_FRAMEWORK_ESP_IDF

#include <aes/esp_aes.h>

namespace esphome {
namespace victron_ble {

static const char *const TAG = "victron_ble";

// defer() coalesces by name: a queued lambda is dropped when a new one is
// queued under the same name. We keep one queue per record type so that
// fast advertisement bursts collapse to the latest reading per category
// rather than blocking unrelated handlers.
static constexpr const char *DEFER_ON_MESSAGE = "victron.on_message";
static constexpr const char *DEFER_SOLAR_CHARGER = "victron.solar_charger";
static constexpr const char *DEFER_BATTERY_MONITOR = "victron.battery_monitor";
static constexpr const char *DEFER_INVERTER = "victron.inverter";
static constexpr const char *DEFER_DCDC_CONVERTER = "victron.dcdc_converter";
static constexpr const char *DEFER_SMART_LITHIUM = "victron.smart_lithium";
static constexpr const char *DEFER_INVERTER_RS = "victron.inverter_rs";
static constexpr const char *DEFER_AC_CHARGER = "victron.ac_charger";
static constexpr const char *DEFER_SMART_BATTERY_PROTECT = "victron.smart_battery_protect";
static constexpr const char *DEFER_LYNX_SMART_BMS = "victron.lynx_smart_bms";
static constexpr const char *DEFER_MULTI_RS = "victron.multi_rs";
static constexpr const char *DEFER_VE_BUS = "victron.ve_bus";
static constexpr const char *DEFER_DC_ENERGY_METER = "victron.dc_energy_meter";
static constexpr const char *DEFER_ORION_XS = "victron.orion_xs";

void VictronBle::dump_config() {
  ESP_LOGCONFIG(TAG, "Victron BLE:");
  ESP_LOGCONFIG(TAG, "  Address: %s", this->address_str().c_str());
}

// Submit update to sensors & callbacks.
void VictronBle::update() {
  if (this->last_package_updated_.exchange(false)) {
    if (this->on_message_callback_.size() > 0) {
      this->defer(DEFER_ON_MESSAGE, [this]() { this->on_message_callback_.call(&this->last_package_); });
    }
    switch (this->last_package_.record_type) {
      case VICTRON_BLE_RECORD_TYPE::SOLAR_CHARGER:
        ESP_LOGD(TAG, "[%s] Received SOLAR_CHARGER message.", this->address_str().c_str());
        if (this->on_solar_charger_message_callback_.size() > 0) {
          this->defer(DEFER_SOLAR_CHARGER, [this]() {
            this->on_solar_charger_message_callback_.call(&this->last_package_.data.solar_charger);
          });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::BATTERY_MONITOR:
        ESP_LOGD(TAG, "[%s] Received BATTERY_MONITOR message.", this->address_str().c_str());
        if (this->on_battery_monitor_message_callback_.size() > 0) {
          this->defer(DEFER_BATTERY_MONITOR, [this]() {
            this->on_battery_monitor_message_callback_.call(&this->last_package_.data.battery_monitor);
          });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::INVERTER:
        ESP_LOGD(TAG, "[%s] Received INVERTER message.", this->address_str().c_str());
        if (this->on_inverter_message_callback_.size() > 0) {
          this->defer(DEFER_INVERTER,
                      [this]() { this->on_inverter_message_callback_.call(&this->last_package_.data.inverter); });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::DCDC_CONVERTER:

        ESP_LOGD(TAG, "[%s] Received DCDC_CONVERTER message.", this->address_str().c_str());
        if (this->on_dcdc_converter_message_callback_.size() > 0) {
          this->defer(DEFER_DCDC_CONVERTER, [this]() {
            this->on_dcdc_converter_message_callback_.call(&this->last_package_.data.dcdc_converter);
          });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::SMART_LITHIUM:
        ESP_LOGD(TAG, "[%s] Received SMART_LITHIUM message.", this->address_str().c_str());
        if (this->on_smart_lithium_message_callback_.size() > 0) {
          this->defer(DEFER_SMART_LITHIUM, [this]() {
            this->on_smart_lithium_message_callback_.call(&this->last_package_.data.smart_lithium);
          });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::INVERTER_RS:
        ESP_LOGD(TAG, "[%s] Received INVERTER_RS message.", this->address_str().c_str());
        if (this->on_inverter_rs_message_callback_.size() > 0) {
          this->defer(DEFER_INVERTER_RS,
                      [this]() { this->on_inverter_rs_message_callback_.call(&this->last_package_.data.inverter_rs); });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::AC_CHARGER:
        ESP_LOGD(TAG, "[%s] Received AC_CHARGER message.", this->address_str().c_str());
        if (this->on_ac_charger_message_callback_.size() > 0) {
          this->defer(DEFER_AC_CHARGER,
                      [this]() { this->on_ac_charger_message_callback_.call(&this->last_package_.data.ac_charger); });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::SMART_BATTERY_PROTECT:
        ESP_LOGD(TAG, "[%s] Received SMART_BATTERY_PROTECT message.", this->address_str().c_str());
        if (this->on_smart_battery_protect_message_callback_.size() > 0) {
          this->defer(DEFER_SMART_BATTERY_PROTECT, [this]() {
            this->on_smart_battery_protect_message_callback_.call(&this->last_package_.data.smart_battery_protect);
          });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::LYNX_SMART_BMS:
        ESP_LOGD(TAG, "[%s] Received LYNX_SMART_BMS message.", this->address_str().c_str());
        if (this->on_lynx_smart_bms_message_callback_.size() > 0) {
          this->defer(DEFER_LYNX_SMART_BMS, [this]() {
            this->on_lynx_smart_bms_message_callback_.call(&this->last_package_.data.lynx_smart_bms);
          });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::MULTI_RS:
        ESP_LOGD(TAG, "[%s] Received MULTI_RS message.", this->address_str().c_str());
        if (this->on_multi_rs_message_callback_.size() > 0) {
          this->defer(DEFER_MULTI_RS,
                      [this]() { this->on_multi_rs_message_callback_.call(&this->last_package_.data.multi_rs); });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::VE_BUS:
        ESP_LOGD(TAG, "[%s] Received VE_BUS message.", this->address_str().c_str());
        if (this->on_ve_bus_message_callback_.size() > 0) {
          this->defer(DEFER_VE_BUS,
                      [this]() { this->on_ve_bus_message_callback_.call(&this->last_package_.data.ve_bus); });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::DC_ENERGY_METER:
        ESP_LOGD(TAG, "[%s] Received DC_ENERGY_METER message.", this->address_str().c_str());
        if (this->on_dc_energy_meter_message_callback_.size() > 0) {
          this->defer(DEFER_DC_ENERGY_METER, [this]() {
            this->on_dc_energy_meter_message_callback_.call(&this->last_package_.data.dc_energy_meter);
          });
        }
        break;
      case VICTRON_BLE_RECORD_TYPE::ORION_XS:
        ESP_LOGD(TAG, "[%s] Received ORION_XS message.", this->address_str().c_str());
        if (this->on_orion_xs_message_callback_.size() > 0) {
          this->defer(DEFER_ORION_XS,
                      [this]() { this->on_orion_xs_message_callback_.call(&this->last_package_.data.orion_xs); });
        }
        break;
      default:
        break;
    }
  }
}

/**
 * Parse all incoming BLE payloads to see if it is a Victron BLE advertisement.
 */

bool VictronBle::parse_device(const esp32_ble_tracker::ESPBTDevice &device) {
  if (device.address_uint64() != this->address_) {
    return false;
  }

  const auto &manu_datas = device.get_manufacturer_datas();
  if (manu_datas.size() != 1) {
    return false;
  }

  const auto &manu_data = manu_datas[0];
  if (manu_data.uuid != esp32_ble_tracker::ESPBTUUID::from_uint16(VICTRON_MANUFACTURER_ID) ||
      manu_data.data.size() <= sizeof(VICTRON_BLE_RECORD_BASE) ||
      manu_data.data.size() > (sizeof(VICTRON_BLE_RECORD_BASE) + VICTRON_ENCRYPTED_DATA_MAX_SIZE)) {
    return false;
  }

  // Parse the unencrypted data.
  const auto *victron_data = (const VICTRON_BLE_RECORD_BASE *) manu_data.data.data();

  if (victron_data->manufacturer_base.manufacturer_record_type !=
      VICTRON_MANUFACTURER_RECORD_TYPE::PRODUCT_ADVERTISEMENT) {
    return false;
  }

  if (victron_data->encryption_key_0 != this->bindkey_[0]) {
    ESP_LOGV(TAG, "[%s] Incorrect Bindkey. Must start with %02X", this->address_str().c_str(), this->bindkey_[0]);
    return false;
  }

  // Filter out duplicate messages
  if ((victron_data->data_counter_lsb | (victron_data->data_counter_msb << 8)) == this->last_package_.data_counter) {
    return false;
  }

  const uint8_t *crypted_data = manu_data.data.data() + sizeof(VICTRON_BLE_RECORD_BASE);
  const uint8_t crypted_len = manu_data.data.size() - sizeof(VICTRON_BLE_RECORD_BASE);
  ESP_LOGVV(TAG, "[%s] Cryted message: %s", this->address_str().c_str(),
            format_hex_pretty(crypted_data, crypted_len).c_str());

  if (!this->is_record_type_supported_(victron_data->record_type, crypted_len)) {
    // Error logging is done by `is_record_type_supported_`.
    return false;
  }

  uint8_t encrypted_data[VICTRON_ENCRYPTED_DATA_MAX_SIZE] = {0};

  if (crypted_len > sizeof(encrypted_data)) {
    ESP_LOGW(TAG, "[%s] Record is too long %u", this->address_str().c_str(), crypted_len);
    return false;
  }

  if (!this->encrypt_message_(crypted_data, crypted_len, encrypted_data, victron_data->data_counter_lsb,
                              victron_data->data_counter_msb)) {
    // Error logging is done by encrypt_message_.
    return false;
  }

  this->handle_record_(victron_data->record_type, encrypted_data);

  // Save the last received data counter
  this->last_package_.data_counter = victron_data->data_counter_lsb | (victron_data->data_counter_msb << 8);
  return true;
}

bool VictronBle::encrypt_message_(const uint8_t *crypted_data, const uint8_t crypted_len,
                                  uint8_t encrypted_data[VICTRON_ENCRYPTED_DATA_MAX_SIZE],
                                  const uint8_t data_counter_lsb, const uint8_t data_counter_msb) {
  esp_aes_context ctx;
  esp_aes_init(&ctx);
  auto status = esp_aes_setkey(&ctx, this->bindkey_.data(), this->bindkey_.size() * 8);
  if (status != 0) {
    ESP_LOGE(TAG, "[%s] Error during esp_aes_setkey operation (%i).", this->address_str().c_str(), status);
    esp_aes_free(&ctx);
    return false;
  }

  // AES-CTR nonce: 16-byte counter block. Victron places the advertisement's
  // 2-byte data counter (little-endian) into bytes 0-1; bytes 2-15 must be
  // zero. Aggregate-init zero-fills omitted entries, but spell out the full
  // layout to keep the nonce structure obvious to future readers.
  size_t nc_offset = 0;
  uint8_t nonce_counter[16] = {data_counter_lsb, data_counter_msb,
                               0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t stream_block[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0};

  status = esp_aes_crypt_ctr(&ctx, crypted_len, &nc_offset, nonce_counter, stream_block, crypted_data, encrypted_data);
  if (status != 0) {
    ESP_LOGE(TAG, "[%s] Error during esp_aes_crypt_ctr operation (%i).", this->address_str().c_str(), status);
    esp_aes_free(&ctx);
    return false;
  }

  esp_aes_free(&ctx);
  ESP_LOGV(TAG, "[%s] Encrypted message: %s", this->address_str().c_str(),
           format_hex_pretty(encrypted_data, crypted_len).c_str());
  return true;
}

bool VictronBle::is_record_type_supported_(const VICTRON_BLE_RECORD_TYPE record_type, const uint8_t crypted_len) {
  uint8_t expected_len = 0;
  switch (record_type) {
    case VICTRON_BLE_RECORD_TYPE::SOLAR_CHARGER:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_SOLAR_CHARGER)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_SOLAR_CHARGER);
      break;
    case VICTRON_BLE_RECORD_TYPE::BATTERY_MONITOR:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_BATTERY_MONITOR)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_BATTERY_MONITOR);
      break;
    case VICTRON_BLE_RECORD_TYPE::INVERTER:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_INVERTER)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_INVERTER);
      break;
    case VICTRON_BLE_RECORD_TYPE::DCDC_CONVERTER:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_DCDC_CONVERTER)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_DCDC_CONVERTER);
      break;
    case VICTRON_BLE_RECORD_TYPE::SMART_LITHIUM:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_SMART_LITHIUM)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_SMART_LITHIUM);
      break;
    case VICTRON_BLE_RECORD_TYPE::INVERTER_RS:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_INVERTER_RS)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_INVERTER_RS);
      break;
    case VICTRON_BLE_RECORD_TYPE::AC_CHARGER:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_AC_CHARGER)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_AC_CHARGER);
      break;
    case VICTRON_BLE_RECORD_TYPE::SMART_BATTERY_PROTECT:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_SMART_BATTERY_PROTECT)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_SMART_BATTERY_PROTECT);
      break;
    case VICTRON_BLE_RECORD_TYPE::LYNX_SMART_BMS:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_LYNX_SMART_BMS)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_LYNX_SMART_BMS);
      break;
    case VICTRON_BLE_RECORD_TYPE::MULTI_RS:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_MULTI_RS)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_MULTI_RS);
      break;
    case VICTRON_BLE_RECORD_TYPE::VE_BUS:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_VE_BUS)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_VE_BUS);
      break;
    case VICTRON_BLE_RECORD_TYPE::DC_ENERGY_METER:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_DC_ENERGY_METER)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_DC_ENERGY_METER);
      break;
    case VICTRON_BLE_RECORD_TYPE::ORION_XS:
      if (crypted_len >= sizeof(VICTRON_BLE_RECORD_ORION_XS)) {
        return true;
      }
      expected_len = sizeof(VICTRON_BLE_RECORD_ORION_XS);
      break;
    default:
      ESP_LOGW(TAG, "[%s] Unsupported record type %02X", this->address_str().c_str(), (uint8_t) record_type);
      return false;
      break;
  }
  ESP_LOGW(TAG, "[%s] Record type %02X message is too short %u, expected %u bytes.", this->address_str().c_str(),
           (uint8_t) record_type, crypted_len, expected_len);
  return false;
}

void VictronBle::handle_record_(const VICTRON_BLE_RECORD_TYPE record_type,
                                const uint8_t encrypted_data[VICTRON_ENCRYPTED_DATA_MAX_SIZE]) {
  this->last_package_.record_type = record_type;
  memcpy(this->last_package_.data.raw, encrypted_data, VICTRON_ENCRYPTED_DATA_MAX_SIZE);
  this->last_package_updated_.store(true);
  this->update();
}

}  // namespace victron_ble
}  // namespace esphome

#endif
