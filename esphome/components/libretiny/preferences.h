#pragma once
#ifdef USE_LIBRETINY

#include "esphome/core/preference_backend.h"
#include <flashdb.h>

namespace esphome::libretiny {

class LibreTinyPreferences final {
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

  struct fdb_kvdb db;
  struct fdb_blob blob;

 protected:
  bool is_changed_(fdb_kvdb_t db, const void *to_save, const char *key_str);
};

void setup_preferences();

}  // namespace esphome::libretiny

namespace esphome {
using Preferences = libretiny::LibreTinyPreferences;
using ESPPreferences = Preferences;
extern ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome

#endif  // USE_LIBRETINY
