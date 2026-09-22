#include "ao_abi.h"
#include "ao/Runtime.hpp"
#include "Session.hpp"

extern "C" int ao_version(char* buf, int buf_len) {
  return ao::version_string(buf, buf_len) == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_runtime_boot(void) {
  return ao::boot() == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_runtime_shutdown(void) {
  return ao::shutdown() == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_image_save(const char* path) {
  return ao::sessionImageSave(path) == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_image_load(const char* path) {
  return ao::sessionImageLoad(path) == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_filein_load_order(const char* path) {
  return ao::sessionFileInLoadOrder(path) == 0 ? AO_OK : AO_ERR;
}
