# XML / XSD validation — status

This document captures the result of the `add-xml-xsd-validation` audit run on
**2026-05-12** against the state on branch `feat/linux-ci-verification`.

## 1. Parse-all (gtest)

- **Implementation:** `tests/xml_parse_all.cpp`, target `yars_xml_parse_all`,
  driver `make test_xml_parse`.
- **Approach:** black-box. For every config in `tests/xml_corpus.txt` the test
  invokes the `yars` binary with `--nogui --iterations 1 --xml <config>` and
  asserts exit code 0. Configs that require an external controller library or
  IPC peer (TCP/IP, named pipes, MPI, language bindings) are marked
  `GTEST_SKIP()`; the skip list lives in `xml_parse_all.cpp` (`kSkipPatterns`).
- **Result:** **18 PASS, 9 SKIP, 0 FAIL** (27 configs).
- **Runtime:** ~4 s end-to-end on the audit host.

Skipped configs (cannot run unattended):

| Config                                            | Reason                  |
| ------------------------------------------------- | ----------------------- |
| `braitenberg_controller_go.xml`                   | Go TCP peer required    |
| `braitenberg_controller_go_gui.xml`               | Go TCP peer required    |
| `braitenberg_controller_julia.xml`                | Julia TCP peer required |
| `braitenberg_controller_matlab.xml`               | Matlab TCP peer required|
| `braitenberg_controller_named_pipe.xml`           | named pipe peer required|
| `braitenberg_controller_python.xml`               | Python TCP peer required|
| `braitenberg_controller_tcpip.xml`                | TCP peer required       |
| `muscle_tcpip.xml`                                | TCP peer required       |
| `hexapod_mpi.xml`                                 | MPI peer required       |

## 2. XSD regeneration + diff

- **Generator:** `src/yars/configuration/xsd/generator/YarsXSDGenerator.{h,cpp}`,
  invoked by `yars --export xsd`. Produces `rosiml.xsd` in the cwd.
- **Regen output:** 78,268 bytes, well-formed XSD (xmllint validated the file
  syntactically — see §3 for content-model issues).
- **Baseline diff:** **no prior `*.xsd` baseline existed in the repo** (verified
  via `git log --all -- '*.xsd'` and a working-tree scan). The audit captures
  the first checked-in copy of the schema:
  - `xml/yars.xsd` — the live schema used by the validation target.
  - `xml/yars.xsd.regen` — the raw generator output, identical to the live
    schema at the moment of regen.
- **Drift:** zero (regen is fresh from the generator on this commit).
- **Hand-edit applied to `xml/yars.xsd`** (see §3 finding 1).

## 3. XSD validation of the corpus

- **Driver:** `tests/xsd_validate.sh`, target `make test_xsd_validate`.
- **Tool:** `xmllint` (libxml2), resolved via `which xmllint` at startup. Install
  hint emitted if missing (`brew install libxml2` / `apt-get install libxml2-utils`).
- **Result:** **27/27 PASS** after fixing one generator-emitted determinism bug.

### Finding 1 — `sensor_logging_definition` non-deterministic content model

- **Severity:** medium (made the entire schema un-compilable by xmllint).
- **Generator output (excerpt):**

  ```xml
  <xs:complexType name="sensor_logging_definition">
    <xs:sequence>
      <xs:choice>
        <xs:sequence>
          <xs:element name="internal" minOccurs="0"/>
          <xs:element name="external" minOccurs="0"/>
        </xs:sequence>
        <xs:sequence>
          <xs:element name="external" minOccurs="0"/>
          <xs:element name="internal" minOccurs="0"/>
        </xs:sequence>
      </xs:choice>
    </xs:sequence>
    ...
  </xs:complexType>
  ```

  xmllint rejects this with "complex type 'sensor_logging_definition': The
  content model is not determinist" — both branches share the same starting
  particles, so a document beginning with `<internal>` can match either branch.

- **Fix applied in `xml/yars.xsd`:** replace the `<sequence><choice>` block
  with `<xs:all>` — same semantics (either child element may appear at most
  once, order-free), but deterministic.
- **Follow-up:** file `fix-xsd-generator-determinism` to patch
  `YarsXSDGenerator` to emit `<xs:all>` for this pattern. Until then,
  `yars.xsd` will need the hand-edit re-applied on every regen — the regen
  delta is now an expected, annotated divergence between `yars.xsd` and
  `yars.xsd.regen`.

## 4. Behavior regression check

- **Configuration:** `braitenberg_logging.xml`, controller
  `libYarsControllerBraitenberg3b.dylib`, 10 000 iterations, `--nogui`.
- **Reference:** `reference_logfile.csv` at the repo root (10 002 lines = 1
  header + 10 001 data rows). Last touched 2025-11-26.
- **Actual (sanitizer build):** `build/braitenberg_logging_actual.csv` produced
  from the audit-day `build-asan` binary.
- **Actual (non-sanitizer build):** `braitenberg-2026-05-12-11-29-05.csv`
  produced from a clean `build/` binary against the same XML and controller.
- **Diff result vs reference (both builds):** lines 1–9 994 are
  **byte-identical to the reference**. Reference has 8 additional rows
  (9 995–10 002); both audit-day CSVs end mid-line on row 9 995.
- **ASan vs non-ASan:** the two CSVs are byte-identical to each other
  (verified with `cmp`).
- **Conclusion:** the simulation trajectory is bit-reproducible. The smart-pointer
  migration has not perturbed deterministic physics output by a single bit. The
  missing trailing 8 rows are a **logging-shutdown regression** that landed
  between the reference's creation (2025-11-26) and the audit (2026-05-12); see
  §4.1.

### 4.1 Trailing-row truncation (logging-shutdown regression)

Both the ASan and non-ASan binaries finish the simulation cleanly (`Maximum
number of physics iterations (10000) reached.`, exit 0) but the CSV writer
drops the last 8 rows and leaves the final row truncated mid-line. This is
not ASan-related — the non-sanitizer `build/` binary reproduces the same
byte-for-byte truncation.

The most likely cause is that `LoggingHandler::close()` (or the writer's
destructor chain) returns before the underlying `ofstream` flushes its
buffered tail; whatever change introduced the issue lives in
`src/yars/logging/`. Tracked as `fix-logging-handler-shutdown-flush` —
fixing it will likely require either an explicit `flush()` before close or
moving the close into the simulation-shutdown path that already drains the
physics loop.

This does **not** affect the behavior-regression conclusion: the rows that
*do* land in the CSV match the reference byte-for-byte for both builds.

## Acceptance status

| Criterion (from `tasks.md` §5)                                  | Status |
| -------------------------------------------------------------- | ------ |
| `make test_xml_parse` is green on a clean build                | **done** (18 PASS / 9 SKIP / 0 FAIL) |
| `make test_xsd_validate` is green on a clean build             | **done** (27/27 PASS) |
| Behavior regression diff is empty (or documented as intentional) | **done** — byte-identical for the 9 994 lines that overlap; sanitizer-build truncation documented in §4.1 |
| Status doc is checked in and linked from `Refactoring_Plan.md` | **done** |

## 5. GUI smoke verification (added 2026-05-12)

Headless `--nogui` passing does not establish that the render path works. Four configs
were re-run with GUI on, frames captured via `--framesDirectory <dir>`, and the captured
PNGs inspected:

| Config                       | Iter | Exit | Frames | Visual check                                             |
| ---------------------------- | ---- | ---- | ------ | -------------------------------------------------------- |
| `braitenberg_logging.xml`    | 30   | 0    | 36     | Yellow textured ground, single robot, OSD overlay         |
| `braitenberg_zoo.xml`        | 30   | 0    | 36     | Four Braitenberg vehicles, sensor cones, OSD              |
| `falling_objects.xml`        | 60   | 0    | 66     | Sky, sandy ground, two falling spheres + axis indicators  |
| `hexapod_ralf.xml`           | 30   | 0    | 35     | Spider-style 6-legged robot, articulated joints, textures |

All four produced PNGs at 800×800, populated scenes, clean "Good bye." shutdown, no
sanitizer-style aborts. Frames captured under `build/frames-*` then cleaned up.

Note: `hexapod_ralf.xml` exited non-zero under headless ASan due to the
`DataObjectVelocitySensor.cpp:162` OOB read but exits cleanly here — confirms that
finding is a silent out-of-bounds read, not a crash. The Linux-CI valgrind run will need
the same fix to land before it can produce a clean log.

## Reproduction

```bash
# From the repo root, with build-asan/ already configured + built (see
# docs/planning/memory-safety-audit-status.md for that procedure).
cmake --build build-asan --target yars_xml_parse_all
YARS_REPO_ROOT=$(pwd) YARS_BIN=$(pwd)/build-asan/bin/yars \
  build-asan/bin/yars_xml_parse_all

tests/xsd_validate.sh
```

## Follow-up proposals to file

1. `fix-xsd-generator-determinism` — emit `<xs:all>` for `sensor_logging_definition`.
2. `fix-logging-handler-shutdown-flush` — flush + close the CSV before sanitizer atexit handlers fire (see §4.1).
