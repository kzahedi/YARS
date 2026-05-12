## 1. Fix

- [x] 1.1 In `DataLoggingSensor::createXsd`, replace the `<choice>{<sequence A>,
  <sequence B>}` block with a single deterministic sequence:
  `<external/>? <internal/>?`. (External-first matches every `<sensor>` usage in
  the bundled corpus; internal-first was attempted first and rejected by the parser.)

## 2. Regen + resync

- [x] 2.1 Rebuild `yars`.
- [x] 2.2 `yars --export xsd` and copy the output over `xml/yars.xsd.regen`.
- [x] 2.3 Confirm `diff xml/yars.xsd xml/yars.xsd.regen` shows only the hand-edit
  removal (no other drift). Then copy regen over `xml/yars.xsd` to remove the
  hand-edit comment.

## 3. Verification

- [x] 3.1 `tests/xsd_validate.sh` reports 27 / 27 PASS on the regenerated schema.
- [x] 3.2 `make test_xml_parse` is unchanged (18 PASS / 9 SKIP / 0 FAIL).
- [x] 3.3 GUI smoke on `braitenberg_logging.xml` produces the expected render and exits
  cleanly.

## 4. Acceptance

- [x] 4.1 `xml/yars.xsd` no longer needs the `<xs:all>` hand-edit comment.
- [x] 4.2 Future `yars --export xsd` runs produce a `yars.xsd.regen` that is identical
  to the live `yars.xsd`.
