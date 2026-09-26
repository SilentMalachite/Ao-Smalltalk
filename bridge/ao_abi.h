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
  AO_ERR_NOSOURCE = 5,
  /* SPEC §3.13: a live-mode evaluation, Proceed or Step stopped its process. */
  AO_ERR_HALT = 6
};

enum {
  AO_EVAL_DOIT = 1,
  AO_EVAL_PRINTIT = 2,
  AO_EVAL_INSPECTIT = 3,
  /* SPEC §3.13: live mode only; stops before the first instruction. */
  AO_EVAL_DEBUGIT = 4
};

/* SPEC §3.10 ライブデバッガの操作 (ao_set_debug_mode). */
enum {
  AO_DEBUG_POSTMORTEM = 0,
  AO_DEBUG_LIVE = 1
};

typedef struct AoSpan {
  unsigned int start;
  unsigned int end;
  char message[256];
} AoSpan;

/* SPEC §3.10. String buffers end in NUL when their length is > 0; an answer that does not fit
   is AO_ERR_RANGE (ao_browser_source's AO_ERR_NOSOURCE wins over it). */

/* SPEC §3.10. The runtime is busy while ao_runtime_boot, ao_runtime_shutdown, ao_image_save,
   ao_image_load, ao_filein_load_order, ao_workspace_reset, ao_eval, ao_accept_method,
   ao_accept_class, ao_debug_frame_receiver_print, ao_debug_frame_temp_print, ao_debug_inspect or
   ao_debug_clear runs, or the interpreter does. Called then (from a transcript or inspect hook,
   or a native), each of these thirteen does nothing and answers AO_ERR: ao_image_load with the
   reason "runtime is busy", ao_eval with an empty out. The running evaluation goes on. The hook
   setters, ao_version, the ao_browser_* reads, ao_eval_result_length, ao_eval_result_copy,
   ao_set_debug_capture and the snapshot reads (ao_debug_generation to ao_debug_frame_temp_name)
   may be called then. No C++ exception leaves any of these functions: it becomes AO_ERR (-1 for
   the *_count functions, ao_eval_result_length, and the ao_debug_* functions below that answer a
   number). */

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

/* SPEC §3.10 デバッガの読み出し, §3.13. Capture copies the stack of a failing evaluation (or of a
   process failing in the drain) into the session's one snapshot before the abort unwinds. Off by
   default; the setting stays across ao_runtime_shutdown, ao_runtime_boot and ao_image_load. */
void ao_set_debug_capture(int on);

/* The snapshot reads; they may be called while busy. Frame i counts from 0, the innermost; temp j
   from 0, the receiver not included. The string reads follow the buffer rule above; AO_ERR also
   for a NULL buf, len < 1, no session or i / j out of range.
   ao_debug_generation: moves by one on every capture and every clear; -1 with no session.
   ao_debug_frame_count: the frames kept (the innermost 256); ao_debug_frame_total: all of them.
   0 with no snapshot, -1 with no session.
   ao_debug_reason: the whole reason, not cut at 255 bytes. AO_ERR with no snapshot.
   ao_debug_frame_kind: 0 method, 1 block, 2 native (synthesized: a failed native or a DNU); -1
   out of range.
   ao_debug_frame_label: "Foo>>bar", "Foo class>>bar", "doIt", "[] in Foo>>bar", "[] in doIt",
   "ArrayedCollection>>at: native ao_ArrayedCollection_at_" (the class the native was found in,
   the selector, the symbol), "#foo (doesNotUnderstand:)".
   ao_debug_frame_pc: the pc of the frame's context (the start of the instruction it runs); -1 for
   kind 2 and out of range.
   ao_debug_frame_source: the source of the frame's method (the home method's for a block, the
   evaluated text for a doIt) and, in highlight (may be NULL), the pc's span in UTF-8 bytes with
   an empty message (0-0 when the pc has none). Without source it writes ao_browser_source's
   placeholder, highlight 0-0, and answers AO_ERR_NOSOURCE (which wins over AO_ERR_RANGE): a
   native frame "\"<Class>>><selector> native <symbol>\"", a DNU frame
   "\"#foo (doesNotUnderstand:) source not available\"".
   ao_debug_frame_temp_count: the arguments and named temps (a block's copied outer variables
   too); for kind 2 the arguments of the send; -1 out of range.
   ao_debug_frame_temp_name: the compiler's name, or arg1, arg2 ... / t1, t2 ... without debug
   info (always argN for kind 2). */
int ao_debug_generation(void);
int ao_debug_frame_count(void);
int ao_debug_frame_total(void);
int ao_debug_reason(char* buf, int len);
int ao_debug_frame_kind(int i);
int ao_debug_frame_label(int i, char* buf, int len);
int ao_debug_frame_pc(int i);
int ao_debug_frame_source(int i, char* buf, int len, AoSpan* highlight);
int ao_debug_frame_temp_count(int i);
int ao_debug_frame_temp_name(int i, int j, char* buf, int len);

/* Outermost entries: AO_ERR while busy. Each clears an earlier abort and takes the stack range
   again first, and reads and clears its own abort last; there is no drain and its abort is not
   captured, so the snapshot and the generation stay.
   The *_print functions write the value's class name to class_buf and its printString to buf
   (both buffers are required); a printString that aborts leaves buf empty and still answers AO_OK
   (AO_ERR_RANGE when either text is cut). ao_debug_inspect calls the inspect hook for the
   receiver (j == -1) or temp j, as Inspect it does; AO_ERR out of range or when inspect or
   printString aborts (the hook is then not called). ao_debug_clear empties the snapshot and moves
   the generation; AO_ERR with no session. */
int ao_debug_frame_receiver_print(int i, char* class_buf, int class_len, char* buf, int len);
int ao_debug_frame_temp_print(int i, int j, char* class_buf, int class_len, char* buf, int len);
int ao_debug_inspect(int i, int j);
int ao_debug_clear(void);
/* AO_ERR when class_name does not name a class (Processor, Smalltalk, an undefined name).
   AO_ERR_COMPILE for a compile error or a refused native overwrite. */
int ao_accept_method(const char* class_name, int meta, const char* source, AoSpan* err);
/* Takes class definition messages and chunk-format class definitions and methodsFor: chunks
   only. Any other chunk: AO_ERR_COMPILE "not a class definition", and nothing is applied. */
int ao_accept_class(const char* source, AoSpan* err);

#ifdef __cplusplus
}
#endif
