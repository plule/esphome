#pragma once
#ifdef USE_ZEPHYR
#ifdef CONFIG_SETTINGS

#include "esphome/core/preference_backend.h"
#include <zephyr/settings/settings.h>
#include <vector>

namespace esphome::zephyr {

class ZephyrPreferences final {
 public:
  void open();
  ESPPreferenceObject make_preference(size_t length, uint32_t type, bool in_flash);
  ESPPreferenceObject make_preference(size_t length, uint32_t type);

  template<typename T, enable_if_t<is_trivially_copyable<T>::value, bool> = true>
  ESPPreferenceObject make_preference(uint32_t type, bool in_flash) {
    return this->make_preference(sizeof(T), type, in_flash);
  }
  template<typename T, enable_if_t<is_trivially_copyable<T>::value, bool> = true>
  ESPPreferenceObject make_preference(uint32_t type) {
    return this->make_preference(sizeof(T), type);
  }

  bool sync();
  bool reset();

 protected:
  std::vector<ZephyrPreferenceBackend *> backends_;

  static int load_setting(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg);
  static int export_settings(int (*cb)(const char *name, const void *value, size_t val_len));
};

void setup_preferences();

}  // namespace esphome::zephyr

namespace esphome {
using Preferences = zephyr::ZephyrPreferences;
using ESPPreferences = Preferences;
extern ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome

#endif  // CONFIG_SETTINGS
#endif  // USE_ZEPHYR
