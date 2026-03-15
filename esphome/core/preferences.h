#pragma once

#include "esphome/core/preference_backend.h"

// Include the concrete preferences manager for the active platform.
// Each header defines its manager class and provides the Preferences,
// ESPPreferences, and global_preferences declarations.
#ifdef USE_ESP32
#include "esphome/components/esp32/preferences.h"
#elif defined(USE_ESP8266)
#include "esphome/components/esp8266/preferences.h"
#elif defined(USE_RP2040)
#include "esphome/components/rp2040/preferences.h"
#elif defined(USE_LIBRETINY)
#include "esphome/components/libretiny/preferences.h"
#elif defined(USE_HOST)
#include "esphome/components/host/preferences.h"
#elif defined(USE_ZEPHYR) && defined(CONFIG_SETTINGS)
#include "esphome/components/zephyr/preferences.h"
#else
namespace esphome {
struct Preferences {
  ESPPreferenceObject make_preference(size_t, uint32_t, bool) { return {}; }
  ESPPreferenceObject make_preference(size_t, uint32_t) { return {}; }
  template<typename T, enable_if_t<is_trivially_copyable<T>::value, bool> = true>
  ESPPreferenceObject make_preference(uint32_t type, bool in_flash) {
    return this->make_preference(sizeof(T), type, in_flash);
  }
  template<typename T, enable_if_t<is_trivially_copyable<T>::value, bool> = true>
  ESPPreferenceObject make_preference(uint32_t type) {
    return this->make_preference(sizeof(T), type);
  }
  bool sync() { return false; }
  bool reset() { return false; }
};
using ESPPreferences = Preferences;
extern ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome
#endif
