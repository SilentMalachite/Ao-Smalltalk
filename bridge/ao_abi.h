#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum {
  AO_OK = 0,
  AO_ERR = 1,
  AO_ERR_COMPILE = 2,
  AO_ERR_EVAL = 3,
  AO_ERR_RANGE = 4,
  AO_ERR_NOSOURCE = 5
};

enum {
  AO_EVAL_DOIT = 1,
  AO_EVAL_PRINTIT = 2,
  AO_EVAL_INSPECTIT = 3
};

typedef struct AoSpan {
  unsigned int start;
  unsigned int end;
  char message[256];
} AoSpan;

/* SPEC §3.10. String buffers end in NUL when their length is > 0; an answer that does not fit
   is AO_ERR_RANGE (ao_browser_source's AO_ERR_NOSOURCE wins over it). */

/* AO_ERR_RANGE when cut (buf still ends in NUL). AO_ERR when buf is NULL or buf_len < 1. */
int ao_version(char* buf, int buf_len);
int ao_runtime_boot(void);
int ao_runtime_shutdown(void);
int ao_image_save(const char* path);
/* Loads into a new session and replaces the current one only when the load and the probes
   (1 + 2, nil isNil) pass. On AO_ERR the current session stays in use, and err (when not NULL)
   gets the reason in message, never empty ("unsupported image version 1", "not an Ao image",
   "image probes failed", ...; SPEC §3.11). On AO_OK the message is empty. */
int ao_image_load(const char* path, AoSpan* err);
int ao_filein_load_order(const char* path);

typedef void (*AoTranscriptFn)(const char* utf8, int len, int is_clear, void* user);
typedef void (*AoInspectFn)(const char* class_name, const char* print_utf8, void* user);

void ao_set_transcript_hook(AoTranscriptFn fn, void* user);
void ao_set_inspect_hook(AoInspectFn fn, void* user);

/* The four *_count functions answer 0 or more, or -1 on failure: no session, a name that is not
   a class, meta other than 0 or 1, a NULL argument. Never AO_ERR, which reads as one row. */
int ao_browser_class_count(void);
int ao_browser_class_at(int index, char* name, int name_len, char* category, int category_len);
int ao_browser_protocol_count(const char* class_name, int meta);
int ao_browser_protocol_at(const char* class_name, int meta, int index, char* buf, int len);
int ao_browser_selector_count(const char* class_name, int meta, const char* protocol);
int ao_browser_selector_at(const char* class_name, int meta, const char* protocol, int index,
                           char* buf, int len);
/* AO_OK with the source when the source table has it. A method without source (a native, a
   method after an image load, a vendor, file-in or methodsFor: chunk method) is AO_ERR_NOSOURCE
   with a one-line comment placeholder that does not compile when accepted:
   "<Class>>><selector> source not available" or "<Class>>><selector> native <symbol>", where
   <Class> is "<Name> class" on the class side. AO_ERR_NOSOURCE also when the placeholder is cut
   (buf still ends in NUL). AO_ERR when buf is NULL, len < 1, or the class or selector is not
   found. */
int ao_browser_source(const char* class_name, int meta, const char* selector, char* buf, int len);
int ao_browser_class_definition(const char* class_name, char* buf, int len);
int ao_browser_superclass(const char* class_name, int meta, char* buf, int len);
int ao_browser_subclass_count(const char* class_name);
int ao_browser_subclass_at(const char* class_name, int index, char* buf, int len);

int ao_workspace_reset(void);
/* out NULL or out_len < 1: AO_ERR before compiling or evaluating anything. A failed evaluation
   is AO_ERR_EVAL with a non-empty err->message. */
int ao_eval(const char* source, int source_len, int mode, char* out, int out_len, AoSpan* err);
/* AO_ERR when class_name does not name a class (Processor, Smalltalk, an undefined name).
   AO_ERR_COMPILE for a compile error or a refused native overwrite. */
int ao_accept_method(const char* class_name, int meta, const char* source, AoSpan* err);
/* Takes class definition messages and chunk-format class definitions and methodsFor: chunks
   only. Any other chunk: AO_ERR_COMPILE "not a class definition", and nothing is applied. */
int ao_accept_class(const char* source, AoSpan* err);

#ifdef __cplusplus
}
#endif
