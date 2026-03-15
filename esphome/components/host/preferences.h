#pragma once
#ifdef USE_HOST

#include "esphome/core/preference_backend.h"
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace esphome::host {

class HostPreferences final {
 public:
  bool sync();
  bool reset();

  ESPPreferenceObject make_preference(size_t length, uint32_t type, bool in_flash);
  ESPPreferenceObject make_preference(size_t length, uint32_t type) { return make_preference(length, type, false); }

  template<typename T, enable_if_t<is_trivially_copyable<T>::value, bool> = true>
  ESPPreferenceObject make_preference(uint32_t type, bool in_flash) {
    return this->make_preference(sizeof(T), type, in_flash);
  }
  template<typename T, enable_if_t<is_trivially_copyable<T>::value, bool> = true>
  ESPPreferenceObject make_preference(uint32_t type) {
    return this->make_preference(sizeof(T), type);
  }

  bool save(uint32_t key, const uint8_t *data, size_t len) {
    if (len > 255)
      return false;
    this->setup_();
    std::vector vec(data, data + len);
    this->data[key] = vec;
    return true;
  }

  bool load(uint32_t key, uint8_t *data, size_t len) {
    if (len > 255)
      return false;
    this->setup_();
    auto it = this->data.find(key);
    if (it == this->data.end())
      return false;
    const auto &vec = it->second;
    if (vec.size() != len)
      return false;
    memcpy(data, vec.data(), len);
    return true;
  }

 protected:
  void setup_();
  bool setup_complete_{};
  std::string filename_{};
  std::map<uint32_t, std::vector<uint8_t>> data{};
};

void setup_preferences();
extern HostPreferences *host_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace esphome::host

namespace esphome {
using Preferences = host::HostPreferences;
using ESPPreferences = Preferences;
extern ESPPreferences *global_preferences;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace esphome

#endif  // USE_HOST
