# JSON Config Migration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make JSON the primary YARS config format via a converter and a JSON parse path, then delete the XML/XSD/Xerces layer; consolidate Data* duplication into declarative binding tables incrementally.

**Architecture:** Strangler in four stages, each its own mergeable branch. Stage 0: generic XML→JSON converter (DOM-level, no per-class code). Stage 1: a JSON reader that *feeds the existing SAX-event API* (`add(DataParseElement*)`) — so every Data* class (~194 files / ~23k LOC in `configuration/data/`, plus the `xsd/` tree; measure the real LOC baseline with `cloc` before Stage 3 so the reduction claim is grounded) parses JSON with zero per-class changes — verified by bit-exact round trips. Stage 2: corpus + CI switch to JSON. Stage 3: delete Xerces/XSD; binding-table consolidation of Data* internals then proceeds class-family-by-class-family with a proven recipe.

**Key insight (deviation from spec, flagged):** the spec ordered "binding tables, then delete XML". This plan gets JSON working end-to-end FIRST via the event-stream adapter (Stage 1), because the Data* classes already consume a stream of parse events — synthesizing that stream from JSON requires one generic walker, not per-class table conversions. Binding tables (Stage 4) then become a pure internal cleanup, decoupled from the format switch and executable incrementally. Same end state, far less risk in the critical path.

**Second flagged deviation:** the spec's Stage 0 is a JSON serializer over the *in-memory data model*; this plan's Stage 0 converts at the raw XML-DOM level instead. The DOM route is string-preserving (no double formatting round-trip) and needs zero per-class code — safer for exact semantics.

**Verified foundations (2026-07-06 review):** the SAX handler emits exactly the assumed contract (`YarsXSDSaxHandler.cpp:16-62`); there is NO text-content path at all (no `characters()` override — element text never reaches Data* classes); Xerces validation only rejects, it never injects XSD defaults (defaults live in the C++ constructors); macro expansion happens inside the Data* layer, so a synthesized stream reproduces it. One lifetime rule the JSON driver must copy: closing elements are consumed synchronously and freed immediately (`YarsXSDSaxHandler.cpp:57-60`) — consumers never retain the pointer.

**Tech Stack:** C++17, nlohmann/json (vendored single header), Xerces-C++ (until Stage 3), existing `DataParseElement` machinery.

**Spec:** `docs/superpowers/specs/2026-07-06-json-config-migration-design.md`

## Global Constraints

- One branch per stage: `feat/json-stage0-converter`, `feat/json-stage1-reader`, `feat/json-stage2-corpus`, `feat/json-stage3-harvest`, `feat/json-stage4-tables`. Each merges (with user approval) before the next starts.
- ALWAYS build/run in `./build`; ALWAYS timeout yars runs; never claim success unless yars compiles and runs.
- The bit-exact CSV gates (braitenberg + hexapod, both platforms) guard every stage.
- macOS: no `grep -P`.
- Read before starting: `src/yars/configuration/xsd/parser/` (SAX parser + `DataParseElement`), `src/yars/configuration/data/Data.h` (root), one representative class pair `DataBox.h/.cpp`.
- No configuration *semantics* change anywhere in this plan: attribute names, defaults, units are frozen.

---

## Stage 0 — XML→JSON converter (generic, DOM-level)

### Task 1: Vendor nlohmann/json

**Files:**
- Create: `ext/json/include/nlohmann/json.hpp` (single-header release)
- Modify: `cmake/IncludePackages.cmake` (add include dir)

- [ ] **Step 1:**

```bash
cd /Volumes/Eregion/projects/yars
mkdir -p ext/json/include/nlohmann
curl -fsSL -o ext/json/include/nlohmann/json.hpp \
  https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
```

Add to `cmake/IncludePackages.cmake`: `include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/ext/json/include")`. Build; then a scratch TU with `#include <nlohmann/json.hpp>` + `nlohmann::json j = {{"ok", true}};` compiles. Commit: `build: vendor nlohmann/json 3.11.3`.

### Task 2: Conversion rules + converter skeleton (TDD)

**Files:**
- Create: `src/yars/configuration/json/XmlToJson.h`, `src/yars/configuration/json/XmlToJson.cpp`
- Create: `tests/unit/test_xml_to_json.cpp` (register in the existing test CMake — see `tests/CMakeLists.txt` for the pattern used by current tests)

**Interfaces:**
- Produces: `nlohmann::json yars::xmlToJson(const std::string &xmlPath)` — mechanical mapping, defined by these rules:
  1. element → JSON object; attributes → string-valued fields (NO type coercion — the Data* classes do their own string parsing today; preserving strings preserves exact semantics, including locale/precision).
  2. child elements → ALWAYS arrays keyed by tag (`"box": [ {...} ]`), even for single children; scalar attributes stay fields. Uglier JSON, zero ambiguity, and Stage 1's reader is trivially symmetric. Document this choice in the file header.
  3. element text content: none exists — the SAX handler has no `characters()` override, so element text never reaches the data layer even today. The converter should REJECT (fail loudly on) non-whitespace text content rather than inventing a representation for it.
  4. **Cross-tag sibling order is load-bearing and MUST be protected up-front, not "decided by test".** `DataRobot` pushes sensors in event-arrival order and exposes them by index — that order is the controller I/O binding (`DataRobot.cpp:182,221`). Grouping by tag reorders INTERLEAVED same-tag siblings (`<proximity/><velocity/><proximity/>` → both proximities adjacent), silently shuffling controller inputs — and the shipped corpus cannot catch it (every corpus config lists children contiguously by type, so round-trip tests pass while the representation stays order-lossy for real user configs). Rule: the converter DETECTS non-contiguous same-tag siblings under any parent and **fails loudly**, naming the parent and line, with a message that such configs need the `#children` ordered representation. (Implementing `#children` is out of scope until a real config needs it; failing loudly is the safety property.) The JSON reader emits arrays in stored order, sibling tag groups in `ordered_json` insertion order — which, for converter-produced files of contiguous documents, reproduces the original event order exactly.

- [ ] **Step 1: Failing test first**

The test suite is **GoogleTest** (NOT Catch2): `tests/CMakeLists.txt` FetchContents googletest v1.14.0 into a single `yars_tests` executable with `gtest_discover_tests`; existing tests use `#include <gtest/gtest.h>`. Add the new file to `yars_tests`' source list in `tests/CMakeLists.txt`.

```cpp
#include <gtest/gtest.h>
#include <yars/configuration/json/XmlToJson.h>
#include <fstream>

TEST(XmlToJson, AttributesBecomeStringFieldsChildrenBecomeArrays)
{
  const char *xml = "<root a=\"1.5\"><child b=\"x\"/><child b=\"y\"/></root>";
  std::string p = testing::TempDir() + "xmltojson_test.xml";
  { std::ofstream f(p); f << xml; }
  auto j = yars::xmlToJson(p);
  EXPECT_EQ(j["root"][0]["a"], "1.5");
  EXPECT_EQ(j["root"][0]["child"].size(), 2u);
  EXPECT_EQ(j["root"][0]["child"][1]["b"], "y");
}

TEST(XmlToJson, InterleavedSameTagSiblingsFailLoudly)
{
  const char *xml = "<root><a x=\"1\"/><b/><a x=\"2\"/></root>";
  std::string p = testing::TempDir() + "xmltojson_interleaved.xml";
  { std::ofstream f(p); f << xml; }
  EXPECT_THROW(yars::xmlToJson(p), std::runtime_error);   // order-lossy input must be rejected
}
```

Run: `cd build && cmake --build . -j 4 && ctest -R XmlToJson` → expect FAIL (unresolved symbol / missing target).

- [ ] **Step 2: Implement** using Xerces DOM (`XercesDOMParser`, already a dependency — parse without schema validation), walking `DOMElement` recursively per the rules above. ~120 lines. Run test → PASS.

- [ ] **Step 3: Commit** `feat(config): generic XML→JSON conversion (string-preserving, always-arrays)`.

### Task 3: `--convert` CLI + corpus conversion smoke

**Files:**
- Modify: CLI definition file (same file as `--nogui`; find via `grep -rn "add_flag" src/yars | head`)
- Modify: `src/yars/main/` early-exit path (before simulation init)
- Create: `scripts/convert-corpus.sh`

**Interfaces:**
- Produces: `yars --convert <file.xml>` writes `<file.json>` next to it and exits 0 without simulating.

- [ ] **Step 1:** Add the flag + early exit (pattern: how `--version` short-circuits — `grep -rn "version" src/yars/main/*.cpp | head`). Implement: call `xmlToJson`, `json::dump(2)`, write file, print path, exit.

- [ ] **Step 2:** `scripts/convert-corpus.sh`: loops all `xml/**/*.xml` configs (skip reference CSVs obviously), runs `--convert` on each with `timeout 30s`, fails on any error. Run it: every corpus config must convert. (Configs that are not full simulation descriptions — e.g. schema fragments — may legitimately fail; exclude by listing, with a comment.)

- [ ] **Step 3:** Commit `feat(cli): --convert xml→json + corpus conversion script`. Do NOT commit generated .json files yet (that's Stage 2).

---

## Stage 1 — JSON reader via the existing parse-event API

### Task 4: Understand and characterize `DataParseElement` (read-only task)

**Files:**
- Create: `docs/planning/json-migration-notes.md`

- [ ] **Step 1:** Read `src/yars/configuration/xsd/parser/` fully: how the SAX handler builds `DataParseElement` (tag name, attributes, opening/closing events) and how `Data*::add(DataParseElement*)` consumes them (state-machine per class: on opening tag X create child, delegate until closing X). Write up in the notes doc: the exact event sequence contract (e.g. does every element produce both an opening and closing event? how are attributes attached? what does `element->closing(TAG)` match on?). Cite file:line for each claim. Commit the notes.

### Task 5: JSON→parse-event driver (TDD)

**Files:**
- Create: `src/yars/configuration/json/JsonParser.h`, `src/yars/configuration/json/JsonParser.cpp`
- Test: `tests/unit/test_json_parser.cpp`

**Interfaces:**
- Consumes: the event contract from Task 4.
- Produces: `bool yars::parseJsonConfig(const std::string &jsonPath, Data *root)` — walks the always-arrays JSON depth-first, synthesizing the identical `DataParseElement` open/close event stream the SAX parser would emit for the equivalent XML, feeding `root->add(...)` exactly as `YarsXSDSaxParser` does.

- [ ] **Step 1: Failing test** — build a minimal JSON (hand-written, matching a tiny XML like Task 2's) and assert the SAME Data tree results from both parse paths. Concretely: pick the smallest real config in the corpus (find it: `wc -l xml/*.xml | sort -n | head -3`), parse it via the existing XML path and via `xmlToJson`+`parseJsonConfig`, then compare observable state (this needs a comparison hook — simplest faithful one: the XSD/`toString` or logging of the tree if one exists; otherwise compare behavior: run 100 sim iterations from both and diff sensor CSVs — the behavioral comparison is the ground truth anyway and needs no new comparison code).

- [ ] **Step 2: Implement the walker** (~150 lines): for each object field that is an array of objects → for each element: emit opening `DataParseElement` with the tag + string attributes, recurse, emit closing. Order: arrays in document order; sibling tags in the order they appear in the JSON object (nlohmann preserves insertion order with `ordered_json` — USE `ordered_json` in both XmlToJson and JsonParser, this matters and is easy to miss).

- [ ] **Step 3: Wire format detection** — in `YarsConfiguration`/wherever the XML path is chosen (`grep -rn "YarsXSDSaxParser" src/yars/configuration/ | head`): `.json` extension → `parseJsonConfig`, else existing path.

- [ ] **Step 3b: Minimal structural validation in the JSON reader (do not skip)**

Today, malformed configs are rejected by Xerces XSD validation BEFORE the Data* classes see them; the Data* `add()` bodies assume required attributes exist (e.g. `DataBox.cpp:67` dereferences `element->attribute(YARS_STRING_KG)` unchecked — nullptr crash if `kg` is missing). Until Stage 4's binding tables provide real validation, the JSON path must not hand malformed input to that machinery unguarded. Add to the walker: unknown-key detection is NOT feasible generically, but null-attribute crashes ARE preventable cheaply — wrap the per-config parse in a try/catch and, additionally, patch `DataParseElement::attribute(name)` misuse at the API boundary: have the JSON driver route through a `DataParseElement` subclass or wrapper that, on a missing-attribute dereference, produces the spec's error format (`file:node: missing attribute 'kg'`) and aborts the load cleanly instead of segfaulting. If that proves invasive, the fallback is documented and user-visible: record in `docs/planning/json-migration-notes.md` that JSON configs are structurally unvalidated until Stage 4 completes, and get explicit user sign-off at the Stage 1 → Stage 2 boundary. Either way the spec's validation promise is only fulfilled when ALL families have binding tables — say so in the notes doc.

- [ ] **Step 4: Corpus round-trip verification (the Stage-1 acceptance)**

```bash
cd /Volumes/Eregion/projects/yars/build
# for each convertible corpus config with a runnable setup — corpus
# sources: tests/xml_corpus.txt, plus the STANDALONE/CFG2LIB arrays in
# .github/workflows/linux-build.yml (and scripts/sanitize-corpus.sh if
# the CI quality gates plan has merged by then):
#   1. run 500 iters from the .xml     -> out-xml.csv (if it logs)
#   2. run 500 iters from the .json    -> out-json.csv
#   3. diff — must be bit-identical
```

Invocation note: the config flag is literally `--xml,-x` (plus a positional `xmlfile`, `ProgramOptions.cpp:37,116`) — JSON files are passed as `yars --xml foo.json`; extension-based detection keys off the filename. stdin mode (`--xml -`) stays XML-only and is out of scope; state that in the code comment.

Write this as `scripts/json-roundtrip-check.sh` (committed). For braitenberg_logging and hexapod_logging this means full CSV diffs; for non-logging configs, exit code + final console output comparison.

- [ ] **Step 5: Commit** `feat(config): JSON parse path via synthesized parse events; corpus round-trip bit-exact`.

---

## Stage 2 — corpus + CI switch

### Task 6: Commit converted corpus, switch CI to JSON

**Files:**
- Create: `xml/*.json` for every runnable corpus config (via `scripts/convert-corpus.sh`)
- Modify: both workflows: audit/corpus/regression steps run the `.json` twins; ADD one step that still runs braitenberg_logging from `.xml` (guards the XML path until Stage 3 deletes it)
- Modify: `README`/`CLAUDE.md` examples to `.json`

- [ ] **Step 1:** Convert, commit configs. **Step 2:** Switch workflow steps (CSV gates now run from JSON — references must stay bit-identical, which Stage 1 already proved locally; if CI disagrees, STOP and debug — do not regenerate references). **Step 3:** Validate YAML, run full local regression from JSON, GUI smoke run from a JSON config. Commit per step; hand off for push approval.

---

## Stage 3 — the harvest

### Task 7: Tag the last XML-capable release, then delete the XML layer

**Files:**
- Delete: `src/yars/configuration/xsd/parser/` (SAX layer), `src/yars/configuration/xsd/generator/` (XSD generator), `src/yars/configuration/xsd/graphviz/` (XsdGraphvizExporter — consumed only by `--export`), XSD schema assets (`src/yars/configuration/xsd/` — inventory first), the `--export xsd|pdf|png` CLI option and `YarsConfiguration::__processExportCommand` (`ProgramOptions.cpp:47`, `YarsConfiguration.cpp:151-172`), the `createXsd(...)` method chain across all Data* classes (headers + bodies — the generator was their only consumer), `XmlToJson.*` and `--convert` (they need Xerces), Xerces from `cmake/IncludePackages.cmake` + workflows (apt `libxerces-c-dev`, brew `xerces-c`)
- Delete/adapt OUTSIDE src/: `xml/yars.xsd` (checked-in schema), `tests/xsd_validate.sh` + the `test_xsd_validate` CMake target, `tests/xml_corpus.txt` + `xml_parse_all.cpp` + `test_xml_parse` (retarget to the JSON corpus rather than delete), `tests/regression/test_braitenberg.cpp` if it loads XML (check first)
- Keep: `DataParseElement` (`src/yars/configuration/data/` — verified it lives there, NOT under xsd/; no move needed); `XmlChangeLog` (`src/yars/configuration/data/XmlChangeLog.{h,cpp}` — used by `--version` and config version gating, survives untouched)
- Modify: `CLAUDE.md`, `docs/planning/v0.8.7-open-points.md`

Note: removing `--export` deletes a user-facing CLI feature (schema/diagram export). Confirm with the user at the same time as the release-tag confirmation in Step 1 — it is XSD-specific by nature, but the removal must be a stated decision, not a silent casualty.

- [ ] **Step 1:** Confirm with the user, then tag: `git tag v0.9.0-last-xml && git push --tags` (approval required). The tag's README section documents `--convert` usage for stragglers.
- [ ] **Step 2:** Delete in dependency order (graphviz/`--export` first, generator + createXsd chain second, SAX third, Xerces last), building between deletions. Completeness gates: `grep -rn "xercesc\|XMLString\|createXsd" src/yars tests --include="*.h" --include="*.cpp" | grep -v json` must end empty (excluding comments), AND `grep -rn "\.xml\b" tests/CMakeLists.txt .github/workflows/` must show only intentionally-retained references.
- [ ] **Step 3:** Full local verification: build, JSON regression bit-exact, GUI smoke, `otool -L build/bin/yars | grep -i xerces` empty. Record LOC delta (`git diff --stat main` summary) in the commit message. Commit per deletion batch.

---

## Stage 4 — binding-table consolidation (incremental, recipe-driven)

### Task 8: Binding-table infrastructure + pilot on the shape family

**Files:**
- Create: `src/yars/configuration/data/DataBinding.h`
- Modify (pilot): `src/yars/configuration/data/DataBox.h/.cpp`, `DataSphere.h/.cpp`, `DataCylinder.h/.cpp`, `DataCapsule.h/.cpp`, `DataPly.h/.cpp`

**Interfaces:**
- Produces: 

```cpp
namespace yars {
// One row per XML/JSON attribute of a Data* class.
struct AttributeBinding {
  const char *name;                                  // attribute name in config
  std::function<void(DataNode*, const std::string&)> apply;  // parse+assign
  bool required;
  const char *defaultValue;                          // nullptr = no default
};
// Drives DataParseElement consumption from a table; replaces the
// hand-written attribute half of add(DataParseElement*).
void applyAttributes(DataNode *self, DataParseElement *element,
                     const std::vector<AttributeBinding> &bindings);
}
```

- [ ] **Step 1:** Write `applyAttributes` + a unit test (a fake DataNode subclass with two attributes, one required, one defaulted; assert values land, missing-required throws with the `file:node: message` format from the spec).
- [ ] **Step 2:** Migrate `DataBox` alone: its `add()` attribute-handling block becomes a static binding table + one `applyAttributes` call; child-element dispatch stays hand-written (that's the state-machine part, tables don't model it — the spec's LOC win comes mostly from `createXsd()` deletion, already harvested in Stage 3, plus attribute-block dedup here). Regression: bit-exact CSVs; boxes appear in the braitenberg environments (NOT in `falling_objects.json`, which is spheres-only) — the standard braitenberg regression gate exercises DataBox directly.
- [ ] **Step 3:** Migrate the remaining four shape classes with the identical recipe. Commit per class.
- [ ] **Step 4: Write the recipe doc** `docs/planning/binding-table-recipe.md`: the mechanical steps + the DataBox before/after diff as the worked example, plus the priority order for remaining families (sensors → actuators → logging → robot/environment roots). Each subsequent family is an independent future task executed with this recipe under the same regression gate — explicitly OK to schedule across sessions.

---

### Final acceptance (whole plan)

- [ ] JSON is the only parse path; Xerces gone from binaries and CI images.
- [ ] Bit-exact CSV gates green from JSON configs on both platforms.
- [ ] GUI runs correctly from a JSON config (project completion bar).
- [ ] `--version` still reports schema/version info correctly (it touches `XmlChangeLog` — verify it survived the deletions; see `docs/planning/v0.8.7-open-points.md` for its history).
- [ ] LOC reduction recorded; open-points doc updated.
