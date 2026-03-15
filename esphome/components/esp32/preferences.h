#pragma once
#ifdef USE_ESP32

#include "esphome/core/preference_backend.h"

namespace esphome::esp32 {

class ESP32Preferences final {
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

  uint32_t nvs_handle;

 protected:
  bool is_changed_(uint32_t nvs_handle, const void *to_save, const char *key_str);
};

void setup_preferences();

}  // namespace esphome::esp32

namespace esphome {
using Preferences = esp32::ESP32Preferences;
using ESPPreferences = Preferences;
extern ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome

#endif  // USE_ESP32
