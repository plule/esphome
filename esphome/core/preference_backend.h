#pragma once

#include <cstring>
#include <cstdint>

#include "esphome/core/defines.h"
#include "esphome/core/helpers.h"

// Include the concrete preference backend for the active platform.
// Each header defines its backend class, forward-declares its manager class,
// declares get_preferences(), and provides the PreferenceBackend alias.
#ifdef USE_ESP32
#include "esphome/components/esp32/preference_backend.h"
#elif defined(USE_ESP8266)
#include "esphome/components/esp8266/preference_backend.h"
#elif defined(USE_RP2040)
#include "esphome/components/rp2040/preference_backend.h"
#elif defined(USE_LIBRETINY)
#include "esphome/components/libretiny/preference_backend.h"
#elif defined(USE_HOST)
#include "esphome/components/host/preference_backend.h"
#elif defined(USE_ZEPHYR) && defined(CONFIG_SETTINGS)
#include "esphome/components/zephyr/preference_backend.h"
#endif

namespace esphome {

#if !defined(USE_ESP32) && !defined(USE_ESP8266) && !defined(USE_RP2040) && !defined(USE_LIBRETINY) && \
    !defined(USE_HOST) && !(defined(USE_ZEPHYR) && defined(CONFIG_SETTINGS))
// Stub for static analysis when no platform is defined.
struct PreferenceBackend {
  bool save(const uint8_t *, size_t) { return false; }
  bool load(uint8_t *, size_t) { return false; }
};
#endif

using ESPPreferenceBackend = PreferenceBackend;

class ESPPreferenceObject {
 public:
  ESPPreferenceObject() = default;
  ESPPreferenceObject(PreferenceBackend *backend) : backend_(backend) {}

  template<typename T> bool save(const T *src) {
    if (this->backend_ == nullptr)
      return false;
    return this->backend_->save(reinterpret_cast<const uint8_t *>(src), sizeof(T));
  }

  template<typename T> bool load(T *dest) {
    if (this->backend_ == nullptr)
      return false;
    return this->backend_->load(reinterpret_cast<uint8_t *>(dest), sizeof(T));
  }

 protected:
  PreferenceBackend *backend_{nullptr};
};

}  // namespace esphome
