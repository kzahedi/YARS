## 1. Sanitizer Build

- [ ] 1.1 Add `YARS_ENABLE_SANITIZERS` CMake option (default OFF)
- [ ] 1.2 Wire `-fsanitize=address,undefined,leak` and `-fno-omit-frame-pointer` when option is ON
- [ ] 1.3 Verify clean compile in `build-asan/` with the option enabled
- [ ] 1.4 Verify the sanitizer-enabled binary runs `braitenberg_nocontroller.xml` for 100 iterations without sanitizer reports unrelated to YARS code (e.g. Ogre/Bullet false positives → suppressions file)

## 2. ASan Run Over Simulation Corpus

- [ ] 2.1 Run each standalone XML config under ASan for 500 iterations `--nogui`
- [ ] 2.2 Run each controller-based XML config under ASan for 500 iterations `--nogui --lib`
- [ ] 2.3 Capture full ASan output to `docs/planning/asan-reports/<config-name>.log`
- [ ] 2.4 Classify each report: leak / UAF / double-free / uninitialized-read / suppressed

## 3. Valgrind Subset (Linux or macOS via brew)

- [ ] 3.1 Confirm valgrind is available; document install command if not
- [ ] 3.2 Run `braitenberg_logging.xml` under valgrind for 100 iterations
- [ ] 3.3 Run `falling_objects.xml` under valgrind for 100 iterations
- [ ] 3.4 Run `braitenberg_zoo.xml` under valgrind for 100 iterations
- [ ] 3.5 Capture output to `docs/planning/valgrind-reports/<config-name>.log`

## 4. Triage and Documentation

- [ ] 4.1 Create `docs/planning/memory-safety-audit-status.md` listing each config and pass/fail status
- [ ] 4.2 For each YARS-attributable defect, file a new OpenSpec change (`fix-<defect>`) or fix inline if a one-line change
- [ ] 4.3 List any third-party suppressions in `suppressions/asan.supp` and `suppressions/valgrind.supp`
- [ ] 4.4 Confirm zero double-free crashes across the audit corpus

## 5. Acceptance

- [ ] 5.1 All configs in scope produce no YARS-attributable ASan reports
- [ ] 5.2 Valgrind subset produces no "definitely lost" leaks attributable to YARS
- [ ] 5.3 Status doc is checked in and linked from `docs/planning/Refactoring_Plan.md`
