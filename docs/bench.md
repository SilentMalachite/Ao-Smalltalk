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

2026-09-23
比 5.76 を再計測していない
起動（ao_runtime_boot と image/vendor/LOAD_ORDER の file-in）: 30 ms
