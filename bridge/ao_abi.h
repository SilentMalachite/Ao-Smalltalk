#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum {
  AO_OK = 0,
  AO_ERR = 1,
  AO_ERR_COMPILE = 2,
  AO_ERR_EVAL = 3,
  AO_ERR_RANGE = 4
};

int ao_version(char* buf, int buf_len);
int ao_runtime_boot(void);
int ao_runtime_shutdown(void);
int ao_image_save(const char* path);
int ao_image_load(const char* path);
int ao_filein_load_order(const char* path);

typedef void (*AoTranscriptFn)(const char* utf8, int len, int is_clear, void* user);

void ao_set_transcript_hook(AoTranscriptFn fn, void* user);

int ao_browser_class_count(void);
int ao_browser_class_at(int index, char* name, int name_len, char* category, int category_len);
int ao_browser_protocol_count(const char* class_name, int meta);
int ao_browser_protocol_at(const char* class_name, int meta, int index, char* buf, int len);
int ao_browser_selector_count(const char* class_name, int meta, const char* protocol);
int ao_browser_selector_at(const char* class_name, int meta, const char* protocol, int index,
                           char* buf, int len);
int ao_browser_source(const char* class_name, int meta, const char* selector, char* buf, int len);
int ao_browser_class_definition(const char* class_name, char* buf, int len);
int ao_browser_superclass(const char* class_name, int meta, char* buf, int len);
int ao_browser_subclass_count(const char* class_name);
int ao_browser_subclass_at(const char* class_name, int index, char* buf, int len);

#ifdef __cplusplus
}
#endif
