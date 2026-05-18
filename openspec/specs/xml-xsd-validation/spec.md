## ADDED Requirements

### Requirement: All bundled XML configs parse successfully
The YARS XML parser SHALL accept every configuration file under `xml/**/*.xml` without raising an error.

#### Scenario: Parse-all test passes
- **WHEN** `make test_xml_parse` is run on a clean build
- **THEN** the gtest reports zero parse failures across the enumerated `xml/` corpus

### Requirement: XSD schema is consistent with the data model
The on-disk `xml/yars.xsd` SHALL match the schema regenerated from the current YARS data model, or any drift SHALL be explicitly documented.

#### Scenario: Regenerated XSD matches checked-in XSD
- **WHEN** the schema is regenerated and diffed against `xml/yars.xsd`
- **THEN** the diff is either empty, or every non-empty diff block is annotated in `docs/planning/xml-xsd-validation-status.md` as intentional

### Requirement: All bundled XML configs validate against the XSD
Every configuration file under `xml/**/*.xml` SHALL pass `xmllint --noout --schema xml/yars.xsd <config>` without validation errors.

#### Scenario: XSD validation test passes
- **WHEN** `make test_xsd_validate` is run
- **THEN** every config in the corpus exits with code 0 from `xmllint --schema`

### Requirement: Reference simulation produces unchanged output
A deterministic simulation (`braitenberg_logging.xml`) at 2 000 iterations SHALL produce CSV output byte-identical to the per-platform reference checked in at `xml/reference_logfile.<platform>.csv`. Supported `<platform>` values: `macos-arm64`, `linux-x86_64`. Cross-platform numerical differences (primarily signed-zero noise in zero-valued velocity columns) are accepted via the per-platform split; same-platform output SHALL be bit-exact.

#### Scenario: Reference logfile diff is empty
- **WHEN** the deterministic reference simulation is run on a supported platform and its CSV output is diffed against the matching `xml/reference_logfile.<platform>.csv`
- **THEN** the diff is empty (CI fails on any difference; the warning-level check from v0.8.7 is gone)

### Requirement: Validation status is documented
A status document SHALL record per-config parse, validate, and behavior-regression results.

#### Scenario: Status document exists
- **WHEN** the XML/XSD validation pass is complete
- **THEN** `docs/planning/xml-xsd-validation-status.md` lists each in-scope config with parse / validate / behavior status and links to the relevant logs and diffs
