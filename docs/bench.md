# P4 microbench

- Machine: Apple Silicon (`Apple M1 Max`)
- Commit: `4e0a725`
- Native `1 to: 10000000 do: [:i | i + 1]` equivalent: 9576 ms
- Interpreter 比: P6 で再測

## P6 interpreter bench

- Machine: Apple Silicon (`Apple M1 Max`)
- Commit: `15af8c3` (interpreter measured here; this section is added in the following commit, which does not change the interpreter)
- Native `makeNativeBlock` path, `1 to: 10000000 do: [:i | i + 1]`: 10666 ms, `interpretedBytecodes` 0
- Bytecode, one send of a user-class method:

```smalltalk
bench
  1 to: 10000000 do: [:i | i + 1]
```

  61449 ms, `interpretedBytecodes` 40000006
- Ratio (bytecode ms / native ms): 5.76 (61449/10666)
- The ratio is under 10. Not optimized.

## B2 `to:do:` bench

- Machine: Apple Silicon (`Apple M1 Max`)
- Date: 2026-09-24
- Commits:
  - `890f281`: before B2. The method sends the native `to:do:`, which sends `value:` to a bytecode block.
  - `c655474`: B2. The literal-block `to:do:` is compiled into jumps. `<=` and `+` are SendSpecial sends.
  - fast path (the commit that adds this section, on `c655474`): two SmallIntegers answer `+ - * < > <= >= =` without a send (SPEC §3.5). The special selectors are interned ahead.
- Builds:
  - Release: `CMAKE_BUILD_TYPE=Release`, `-O3 -DNDEBUG -g`
  - Debug: `build/`. `CMAKE_BUILD_TYPE` is empty in the cache, so `CMakeLists.txt` uses Debug (`-g`, no optimization, asserts on, no sanitizer). The P6 61449 ms is from this configuration.
- Method: a C++ harness (not in the repository) links the runtime libraries, boots, and files in `image/vendor/LOAD_ORDER`, outside the timing. N = 10000000. Median of 3 runs.
  - method: one send of the user-class method below (accept is outside the timing)
  - workspace: `ao_eval` (doIt) of `1 to: 10000000 do: [:i | i + 1]`, compile included

```smalltalk
bench
  1 to: 10000000 do: [:i | i + 1]
```

ms (ns per iteration):

| Build | Eval | `890f281` | `c655474` | fast path |
|---|---|---|---|---|
| Release | method | 3099 (310) | 5975 (598) | 2606 (261) |
| Release | workspace | 3089 (309) | 5988 (599) | 2611 (261) |
| Debug | method | 62499 (6250) | 110770 (11077) | 38858 (3886) |
| Debug | workspace | 64021 (6402) | 114180 (11418) | 38562 (3856) |

- `interpretedBytecodes` (method / workspace): `890f281` 40000006 / 40000005 (4 per iteration). `c655474` and the fast path 130000011 / 130000010 (13 per iteration). The fast path makes no send in the loop (`interpretedSends` 0).
- Fast path: Release 0.44 × `c655474`, 0.84 × `890f281`. Debug 0.35 × `c655474`, 0.62 × `890f281`.
- `890f281` Release, measured again in the same session as the fast path: method 3106 ms, workspace 3089 ms.
- Release `sample` of the fast path: 98 % of the loop is `Interpreter::run` and what it calls (Heap accessors 42 %, `run` itself 38 %, operand stack with `Roots` 14 %).
- Ratio 5.76 (P6): it compared the native `to:do:` with a native block and with a bytecode block. Since B2 the method compiles `to:do:` into jumps and makes no block, so the ratio is not recomputed; compare commits in the table above instead.
- Startup (`ao_runtime_boot` and the `image/vendor/LOAD_ORDER` file-in): 30 ms (2026-09-23)
