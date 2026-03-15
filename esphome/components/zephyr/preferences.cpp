#ifdef USE_ZEPHYR
#ifdef CONFIG_SETTINGS

#include <zephyr/kernel.h>
#include "preferences.h"
#include "esphome/core/log.h"
#include <zephyr/settings/settings.h>
#include <cinttypes>
#include <cstring>

namespace esphome::zephyr {

static const char *const TAG = "zephyr.preferences";

bool ZephyrPreferenceBackend::save(const uint8_t *data, size_t len) {
  this->data.resize(len);
  std::memcpy(this->data.data(), data, len);
  ESP_LOGVV(TAG, "save key: %u, len: %d", this->type_, len);
  return true;
}

bool ZephyrPreferenceBackend::load(uint8_t *data, size_t len) {
  if (len != this->data.size()) {
    char key_buf[KEY_BUFFER_SIZE];
    this->format_key(key_buf, sizeof(key_buf));
    ESP_LOGE(TAG, "size of setting key %s changed, from: %u, to: %u", key_buf, this->data.size(), len);
    return false;
  }
  std::memcpy(data, this->data.data(), len);
  ESP_LOGVV(TAG, "load key: %u, len: %d", this->type_, len);
  return true;
}

void ZephyrPreferences::open() {
  int err = settings_subsys_init();
  if (err) {
    ESP_LOGE(TAG, "Failed to initialize settings subsystem, err: %d", err);
    return;
  }

  static struct settings_handler settings_cb = {
      .name = ESPHOME_SETTINGS_KEY,
      .h_set = load_setting,
      .h_export = export_settings,
  };

  err = settings_register(&settings_cb);
  if (err) {
    ESP_LOGE(TAG, "setting_register failed, err, %d", err);
    return;
  }

  err = settings_load_subtree(ESPHOME_SETTINGS_KEY);
  if (err) {
    ESP_LOGE(TAG, "Cannot load settings, err: %d", err);
    return;
  }
  ESP_LOGD(TAG, "Loaded %u settings.", this->backends_.size());
}

ESPPreferenceObject ZephyrPreferences::make_preference(size_t length, uint32_t type, bool in_flash) {
  return make_preference(length, type);
}

ESPPreferenceObject ZephyrPreferences::make_preference(size_t length, uint32_t type) {
  for (auto *backend : this->backends_) {
    if (backend->get_type() == type) {
      return ESPPreferenceObject(backend);
    }
  }
  printf("type %u size %u\n", type, this->backends_.size());
  auto *pref = new ZephyrPreferenceBackend(type);  // NOLINT(cppcoreguidelines-owning-memory)
  char key_buf[KEY_BUFFER_SIZE];
  pref->format_key(key_buf, sizeof(key_buf));
  ESP_LOGD(TAG, "Add new setting %s.", key_buf);
  this->backends_.push_back(pref);
  return ESPPreferenceObject(pref);
}

bool ZephyrPreferences::sync() {
  ESP_LOGD(TAG, "Save settings");
  int err = settings_save();
  if (err) {
    ESP_LOGE(TAG, "Cannot save settings, err: %d", err);
    return false;
  }
  return true;
}

bool ZephyrPreferences::reset() {
  ESP_LOGD(TAG, "Reset settings");
  for (auto *backend : this->backends_) {
    // save empty delete data
    backend->data.clear();
  }
  this->sync();
  return true;
}

int ZephyrPreferences::load_setting(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
  auto type = parse_hex<uint32_t>(name);
  if (!type.has_value()) {
    std::string full_name(ESPHOME_SETTINGS_KEY);
    full_name += "/";
    full_name += name;
    // Delete unusable keys. Otherwise it will stay in flash forever.
    settings_delete(full_name.c_str());
    return 1;
  }
  std::vector<uint8_t> data(len);
  int err = read_cb(cb_arg, data.data(), len);

  ESP_LOGD(TAG, "load setting, name: %s(%u), len %u, err %u", name, *type, len, err);
  auto *pref = new ZephyrPreferenceBackend(*type, std::move(data));  // NOLINT(cppcoreguidelines-owning-memory)
  static_cast<ZephyrPreferences *>(global_preferences)->backends_.push_back(pref);
  return 0;
}

int ZephyrPreferences::export_settings(int (*cb)(const char *name, const void *value, size_t val_len)) {
  for (auto *backend : static_cast<ZephyrPreferences *>(global_preferences)->backends_) {
    char name[KEY_BUFFER_SIZE];
    backend->format_key(name, sizeof(name));
    int err = cb(name, backend->data.data(), backend->data.size());
    ESP_LOGD(TAG, "save in flash, name %s, len %u, err %d", name, backend->data.size(), err);
  }
  return 0;
}

static ZephyrPreferences s_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

ZephyrPreferences *get_preferences() { return &s_preferences; }

void setup_preferences() {
  global_preferences = &s_preferences;
  s_preferences.open();
}

}  // namespace esphome::zephyr

namespace esphome {
ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome

#endif
#endif
