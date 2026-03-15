#pragma once
#ifdef USE_ESP8266

#include "esphome/core/preference_backend.h"

namespace esphome::esp8266 {

class ESP8266Preferences final {
 public:
  void setup();
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

  uint32_t current_offset = 0;
  uint32_t current_flash_offset = 0;  // in words
};

void setup_preferences();
void preferences_prevent_write(bool prevent);

}  // namespace esphome::esp8266

namespace esphome {
using Preferences = esp8266::ESP8266Preferences;
using ESPPreferences = Preferences;
extern ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome

#endif  // USE_ESP8266
