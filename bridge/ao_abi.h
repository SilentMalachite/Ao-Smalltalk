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

/* SPEC §3.10. The runtime is busy while ao_runtime_boot, ao_runtime_shutdown, ao_image_save,
   ao_image_load, ao_filein_load_order, ao_workspace_reset, ao_eval, ao_accept_method or
   ao_accept_class runs, or the interpreter does. Called then (from a transcript or inspect hook,
   or a native), each of these nine does nothing and answers AO_ERR: ao_image_load with the reason
   "runtime is busy", ao_eval with an empty out. The running evaluation goes on. The hook setters,
   ao_version, the ao_browser_* reads, ao_eval_result_length and ao_eval_result_copy may be called
   then. No C++ exception leaves any of these functions: it becomes AO_ERR (-1 for the *_count
   functions and ao_eval_result_length). */

/* AO_ERR_RANGE when cut (buf still ends in NUL). AO_ERR when buf is NULL or buf_len < 1. */
int ao_version(char* buf, int buf_len);
int ao_runtime_boot(void);
int ao_runtime_shutdown(void);
/* SPEC §3.11. An image this answers AO_OK for passes this runtime's load checks: the bytes are
   checked before anything is written, and a heap that breaks a check is AO_ERR. The file goes to
   a temporary .aoimage-XXXXXX next to the file it replaces (a symbolic link at path is followed
   and stays a link), is synced and renamed over it. On AO_ERR (a check, an existing file that is
   not writable, a failed write) the file at path is left as it was. After the rename the save has
   succeeded; the directory sync that follows is best effort. */
int ao_image_save(const char* path);
/* Loads into a new session and replaces the current one only when the load and the probes
   (1 + 2, nil isNil) pass. On AO_ERR the current session stays in use, and err (when not NULL)
   gets the reason in message, never empty ("unsupported image version 1", "not an Ao image",
   "image probes failed", ...; SPEC §3.11). On AO_OK the message is empty. */
int ao_image_load(const char* path, AoSpan* err);
int ao_filein_load_order(const char* path);

typedef void (*AoTranscriptFn)(const char* utf8, int len, int is_clear, void* user);
/* print_len is the byte count of print_utf8, which may hold NUL bytes; a NUL follows it too. */
typedef void (*AoInspectFn)(const char* class_name, const char* print_utf8, int print_len,
                            void* user);

/* The hook stays set across ao_runtime_shutdown, ao_runtime_boot and ao_image_load, and may be
   set before the first boot. NULL removes it. */
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
/* SPEC §3.10, the evaluation result. The last ao_eval's result: the whole printString of a
   Print it or Inspect it that answered AO_OK or AO_ERR_RANGE (read it past out), else empty. An
   ao_eval refused while busy leaves it; boot and a load start empty. It may hold NUL bytes, so
   read it by its length. ao_eval_result_length answers the byte count, or -1 with no session or
   no result (a printString of INT_MAX bytes or more is not kept). ao_eval_result_copy writes it
   like the other string buffers (AO_ERR_RANGE when cut, buf still ends in NUL); AO_ERR when buf
   is NULL, buf_len < 1, or there is no session or no result. */
int ao_eval_result_length(void);
int ao_eval_result_copy(char* buf, int buf_len);
/* AO_ERR when class_name does not name a class (Processor, Smalltalk, an undefined name).
   AO_ERR_COMPILE for a compile error or a refused native overwrite. */
int ao_accept_method(const char* class_name, int meta, const char* source, AoSpan* err);
/* Takes class definition messages and chunk-format class definitions and methodsFor: chunks
   only. Any other chunk: AO_ERR_COMPILE "not a class definition", and nothing is applied. */
int ao_accept_class(const char* source, AoSpan* err);

#ifdef __cplusplus
}
#endif
