## 1. Fix

- [x] 1.1 In `YarsXSDSaxParser::read`, transcode `"file:///schema.xsd"` via
  `XMLString::transcode` to obtain an `XMLCh*`, pass it to `setProperty`, then release
  with `XMLString::release(&id)` after `loadGrammar`.

## 2. Verification

- [x] 2.1 Rebuild `build-asan/`.
- [x] 2.2 Re-run `braitenberg_nocontroller.xml` (100 iter, `--nogui`, ASan on) and confirm
  the only previous finding (XSD-parser-1) is gone.
- [x] 2.3 Re-run the four ASan-clean configs that exposed only this finding (e.g.
  `braitenberg.xml`, `falling_objects.xml`) and confirm the logs now contain no
  `ERROR: AddressSanitizer` lines attributable to YARS.
- [x] 2.4 Update `docs/planning/asan-reports/<config>.log` with the post-fix output.
- [x] 2.5 Run `make test_xml_parse` and confirm 18 PASS / 9 SKIP / 0 FAIL is preserved.

## 3. Acceptance

- [x] 3.1 No `ERROR: AddressSanitizer: global-buffer-overflow … setProperty …` reports
  remain in the audit corpus.
- [x] 3.2 `test_xml_parse` still green.
- [x] 3.3 GUI smoke on `braitenberg_logging.xml` still produces a clean window and `Good
  bye.` shutdown.
