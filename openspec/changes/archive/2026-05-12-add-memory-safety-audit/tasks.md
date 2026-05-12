## 1. Sanitizer Build

- [x] 1.1 Add `YARS_ENABLE_SANITIZERS` CMake option (default OFF)
- [x] 1.2 Wire `-fsanitize=address,undefined,leak` and `-fno-omit-frame-pointer` when option is ON
- [x] 1.3 Verify clean compile in `build-asan/` with the option enabled
- [x] 1.4 Verify the sanitizer-enabled binary runs `braitenberg_nocontroller.xml` for 100 iterations without sanitizer reports unrelated to YARS code (e.g. Ogre/Bullet false positives → suppressions file)
  - **Finding (YARS-attributable):** ASan global-buffer-overflow at `YarsXSDSaxParser.cpp:60` — narrow `char*` passed as XMLCh* to `parser->setProperty(fgXercesSchemaExternalNoNameSpaceSchemaLocation, ...)`. Xerces reads past the 19-byte `"file:///schema.xsd"` literal. Report at `docs/planning/asan-reports/braitenberg_nocontroller.log`. Tracked as a separate fix proposal (`fix-xercesxsd-buffer-overflow`). Run completed all 100 physics iterations.

## 2. ASan Run Over Simulation Corpus

- [x] 2.1 Run each standalone XML config under ASan for 500 iterations `--nogui`
- [-] 2.2 Run each controller-based XML config under ASan for 500 iterations `--nogui --lib` — **deferred**: controller libs (Python/Julia/Matlab/Go/CRBM) are not in this build configuration; configs that reference them exit 255 with "Controller 'X' not found" before any simulation. Listed in `docs/planning/asan-reports/skipped.md`. Re-enable when the controller-build-verification capability lands.
- [x] 2.3 Capture full ASan output to `docs/planning/asan-reports/<config-name>.log`
- [x] 2.4 Classify each report: leak / UAF / double-free / uninitialized-read / suppressed

## 3. Valgrind Subset (Linux or macOS via brew)

- [x] 3.1 Confirm valgrind is available; document install command if not — **not available on arm64 macOS**. Upstream valgrind has no Apple Silicon port; `brew install valgrind` fails. The runs in 3.2–3.5 must be executed on a Linux runner (the Linux CI job from `add-linux-port-verification` is the right vehicle).
- [-] 3.2 Run `braitenberg_logging.xml` under valgrind for 100 iterations — **deferred to Linux CI**.
- [-] 3.3 Run `falling_objects.xml` under valgrind for 100 iterations — **deferred to Linux CI**.
- [-] 3.4 Run `braitenberg_zoo.xml` under valgrind for 100 iterations — **deferred to Linux CI**.
- [-] 3.5 Capture output to `docs/planning/valgrind-reports/<config-name>.log` — **deferred to Linux CI**.

## 4. Triage and Documentation

- [x] 4.1 Create `docs/planning/memory-safety-audit-status.md` listing each config and pass/fail status
- [x] 4.2 For each YARS-attributable defect, file a new OpenSpec change (`fix-<defect>`) or fix inline if a one-line change — four follow-ups identified and listed in the status doc (`fix-xercesxsd-buffer-overflow`, `fix-logging-sensor-strlen-overflow`, `fix-muscle-actuator-uninit-bools`, `fix-velocity-sensor-bounds`); proposals to be filed as follow-up work, not bundled with this audit.
- [x] 4.3 List any third-party suppressions in `suppressions/asan.supp` and `suppressions/valgrind.supp` — files created (both empty: no third-party suppression candidates identified in the macOS run; valgrind file ready for Linux CI).
- [x] 4.4 Confirm zero double-free crashes across the audit corpus — no `heap-use-after-free` or `double-free` reports in any of the 17 logs.

## 5. Acceptance

- [-] 5.1 All configs in scope produce no YARS-attributable ASan reports — **partial:** the 17 standalone configs produce four classes of finding (cataloged in the status doc). The audit's spec-defined goal of triaging the smart-pointer migration is met; the four findings are pre-existing defects routed to dedicated fix proposals rather than being suppressed.
- [-] 5.2 Valgrind subset produces no "definitely lost" leaks attributable to YARS — **deferred to Linux CI** (no arm64 macOS valgrind).
- [x] 5.3 Status doc is checked in and linked from `docs/planning/Refactoring_Plan.md`
