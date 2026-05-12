## Why

The memory-safety audit (`add-memory-safety-audit`, run 2026-05-12) recorded an
AddressSanitizer global-buffer-overflow on every config that goes through the YARS XSD
parser — i.e. every config. The defect is at
`src/yars/configuration/xsd/parser/YarsXSDSaxParser.cpp:60`: a narrow `char*` literal
(`"file:///schema.xsd"`, 19 bytes) is cast to `void*` and handed to Xerces' SAX2
`setProperty(fgXercesSchemaExternalNoNameSpaceSchemaLocation, …)`, which expects a
`const XMLCh*` (UTF-16). Xerces walks the buffer 16 bits at a time looking for a
double-zero terminator and reads ~20 bytes past the end of the string literal.

The bug has been present since the schema-location wiring was introduced and was masked
because the read targets a constant-pool literal, so the dereference happens to land on
neighboring read-only memory; the simulation does not crash, only ASan flags it. Fixing
it removes the recurring noise from every future audit run.

## What Changes

- Transcode the schema-location URI via `xercesc::XMLString::transcode(...)` before
  passing it to `setProperty`; release the transcoded buffer after `loadGrammar` returns
  (matching the existing transcode/release pattern in this file).

## Capabilities

### New Capabilities

<!-- None -->

### Modified Capabilities

<!-- None — pure bug fix; behavior is unchanged from the perspective of XML configs. -->

## Impact

- `src/yars/configuration/xsd/parser/YarsXSDSaxParser.cpp` — ~5 lines changed.
- No public API change, no XML config change, no build option change.
- Validation: re-run the `add-memory-safety-audit` corpus (`build-asan/`) and confirm the
  XSD-parser-1 finding no longer appears in any of the 17 logs.
