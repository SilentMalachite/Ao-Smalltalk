#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum {
  AO_OK = 0,
  AO_ERR = 1
};

int ao_version(char* buf, int buf_len);
int ao_runtime_boot(void);
int ao_runtime_shutdown(void);
int ao_image_save(const char* path);
int ao_image_load(const char* path);
int ao_filein_load_order(const char* path);

typedef void (*AoTranscriptFn)(const char* utf8, int len, int is_clear, void* user);

void ao_set_transcript_hook(AoTranscriptFn fn, void* user);

#ifdef __cplusplus
}
#endif
