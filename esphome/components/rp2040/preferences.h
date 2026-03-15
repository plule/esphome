#pragma once
#ifdef USE_RP2040

#include "esphome/core/preference_backend.h"

namespace esphome::rp2040 {

class RP2040Preferences final {
 public:
  RP2040Preferences();
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

  uint32_t current_flash_offset = 0;

 protected:
  uint8_t *eeprom_sector_;
};

void setup_preferences();
void preferences_prevent_write(bool prevent);

}  // namespace esphome::rp2040

namespace esphome {
using Preferences = rp2040::RP2040Preferences;
using ESPPreferences = Preferences;
extern ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome

#endif  // USE_RP2040
