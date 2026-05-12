## Why

Phase 3 of the modernization plan called for refactoring the XML configuration system, including parser cleanup and XSD schema regeneration. The plan's validation checklist (all XML configs parse, XSD generation produces valid schemas, no regression in simulation behavior) was never closed out. `simulation-audit` confirms simulations run, but it does not specifically verify that the XML parser accepts every config and that the on-disk XSD schema is consistent with the parsed data model. This change closes the validation gap before any further Phase 3 work (parser simplification, generated DataNode classes) begins.

## What Changes

- **Parse-all check**: A test runner attempts to parse every `xml/**/*.xml` config that ships with YARS and reports any failure
- **XSD regeneration**: The XSD schema is regenerated from the current data model and diffed against the checked-in `yars.xsd` — drift is documented
- **XSD-validate the corpus**: Every XML config is validated against the regenerated XSD using `xmllint --schema yars.xsd <config>`
- **Output diff vs reference**: A subset of configs is run with deterministic logging output (e.g. `braitenberg_logging.xml`) and the CSV output is diffed against the reference logfile to confirm no behavior regression
- **Document the baseline**: A status doc records pass/fail per config and the diff result vs reference output

## Capabilities

### New Capabilities

- `xml-xsd-validation`: Parser and schema correctness verification across the bundled XML configuration corpus, plus a behavior-regression check vs reference output

### Modified Capabilities

<!-- None. `simulation-audit` covers crash-free runs but does not cover parser-level validation or schema correctness; the two capabilities are complementary -->

## Impact

- `tests/xml_parse_all.cpp` — new gtest that loads every config in `xml/`
- `tests/xsd_validate.sh` — new shell script driving `xmllint --schema`
- `xml/yars.xsd` — may be regenerated; diff is captured before any change is merged
- `docs/planning/xml-xsd-validation-status.md` — new status doc
- `xml/reference_logfile.csv` — used as the regression baseline; no change unless intentional
- Build/CMake: new `make test_xml` target wiring the gtest and shell-script suite
