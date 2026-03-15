#pragma once
#ifdef USE_LIBRETINY

#include <cstddef>
#include <cstdint>

// Forward declare FlashDB types to avoid pulling in flashdb.h
struct fdb_kvdb;
typedef struct fdb_kvdb *fdb_kvdb_t;
struct fdb_blob;
typedef struct fdb_blob *fdb_blob_t;

namespace esphome::libretiny {

class LibreTinyPreferenceBackend final {
 public:
  bool save(const uint8_t *data, size_t len);
  bool load(uint8_t *data, size_t len);

  uint32_t key;
  fdb_kvdb_t db;
  fdb_blob_t blob;
};

class LibreTinyPreferences;
LibreTinyPreferences *get_preferences();

}  // namespace esphome::libretiny

namespace esphome {
using PreferenceBackend = libretiny::LibreTinyPreferenceBackend;
}  // namespace esphome

#endif  // USE_LIBRETINY
