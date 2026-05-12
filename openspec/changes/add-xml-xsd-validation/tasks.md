## 1. Parse-All Test

- [ ] 1.1 Enumerate every `xml/**/*.xml` config and write the list to `tests/xml_corpus.txt`
- [ ] 1.2 Create `tests/xml_parse_all.cpp` using gtest that loads each config via the YARS XML parser
- [ ] 1.3 Wire `make test_xml_parse` to compile and run the gtest
- [ ] 1.4 Achieve green: every config parses without error (or document the exception and exclude with a tracking comment)

## 2. XSD Regeneration and Diff

- [ ] 2.1 Locate the XSD generator (legacy tool or manual maintenance) and confirm it builds
- [ ] 2.2 Regenerate `yars.xsd` from the current data model into `xml/yars.xsd.regen`
- [ ] 2.3 `diff xml/yars.xsd xml/yars.xsd.regen` and capture to `docs/planning/xml-xsd-validation-status.md`
- [ ] 2.4 Decide per-diff-block: accept (commit regen as new yars.xsd) or document drift as intentional

## 3. XSD Validation of the Corpus

- [ ] 3.1 Confirm `xmllint` is available; document install if not (`brew install libxml2`)
- [ ] 3.2 Create `tests/xsd_validate.sh` that runs `xmllint --noout --schema xml/yars.xsd <config>` for every config in the corpus
- [ ] 3.3 Wire `make test_xsd_validate` to invoke the shell script and aggregate results
- [ ] 3.4 Achieve green: every in-scope config validates against the (possibly regenerated) XSD

## 4. Behavior Regression Check

- [ ] 4.1 Run `yars --iterations <reference-count> --nogui --xml xml/braitenberg_logging.xml --lib <contrib_lib>` and capture CSV output to `build/braitenberg_logging_actual.csv`
- [ ] 4.2 `diff xml/reference_logfile.csv build/braitenberg_logging_actual.csv` — must be byte-identical (or whitespace-only difference, documented)
- [ ] 4.3 If diff is non-empty: classify as intentional (update reference) or regression (file a fix change)

## 5. Acceptance

- [ ] 5.1 `make test_xml_parse` is green on a clean build
- [ ] 5.2 `make test_xsd_validate` is green on a clean build
- [ ] 5.3 Behavior regression diff is empty (or documented as intentional)
- [ ] 5.4 `docs/planning/xml-xsd-validation-status.md` is checked in and linked from `Refactoring_Plan.md`
