## Why

The memory-safety audit (`add-memory-safety-audit`, run 2026-05-12) recorded an
AddressSanitizer heap-buffer-overflow in `LoggingModuleSensor::__useExternal` and the
mirrored `__useInternal` — they allocate a single byte and immediately pass it to
`std::string(const char*)`, which performs `strlen` on a buffer that has no null
terminator and walks off the end.

```cpp
char* c = new char[1];      // 1 byte, never null-terminated
c[0] = (char)index;          // value of `index` stored as a char
string s(c);                 // strlen(c) reads past the end → ASan finding
addMethod(..., s);
delete[] c;
```

The "string" is used downstream as an opaque token keyed off the integer value of
`index`. The intent is clear; the implementation is wrong.

## What Changes

- Replace the `new char[1]` + `string(const char*)` trick with the size-aware constructor
  `std::string(1, static_cast<char>(index))`, dropping the manual heap allocation and the
  matching `delete[]`.
- Apply the same fix to both `__useExternal` and `__useInternal` in
  `src/yars/logging/LoggingModuleSensor.cpp`.

## Capabilities

### New Capabilities

<!-- None -->

### Modified Capabilities

<!-- None — logging behavior is unchanged: the token sent to `addMethod` is the same
single-character string. -->

## Impact

- `src/yars/logging/LoggingModuleSensor.cpp` — ~6 lines changed, no headers touched.
- Validation: rerun `braitenberg_logging.xml`, `braitenberg_noise.xml`, `hexapod_ralf.xml`
  under ASan and confirm the heap-buffer-overflow in `LoggingModuleSensor` no longer
  appears.
