# Road to 1.0 — remaining work plan (2026-07-10)

Everything else on the modernization roadmap has shipped (JSON format
v0.14, binding tables, required-attribute errors, noise ownership,
Vulkan/macros removal, submodule hosting). What remains is a bounded
ownership burn-down plus two optional streams. When Phase 1–3 are done,
`scripts/sanitizer-suppressions.txt` contains only entries that are
*documented as intentional* — that is the proposed 1.0 gate.

Every phase uses the proven flow: work on a `feat/**` branch, one
class/entry per commit, local gates after every step (yars_tests,
braitenberg + hexapod reference CSVs, roundtrip 13/13), Linux CI
sanitize job as the merge gate (LeakSanitizer does not run on macOS).

## Phase 1 — finish the noise/filter ownership family (small)

Same disease, same cure as the shipped `DataSensor::_noise` redesign.

1.1 **`DataGenericActuatorSensor`** (`vector<DataNoise*> _noise`,
    `vector<DataFilter*> _filter` shadowing the base members):
    convert both to `std::vector<std::unique_ptr<...>>`; adapt the
    per-axis add()/copy() paths; `.get()` at factory call sites.
    No suppression names this class — the win is correctness and
    removing the shadowing trap the noise agent flagged.

1.2 **`_filter` members everywhere** (~15 sensor classes + actuators):
    mechanical repeat of the `_noise` transform —
    `std::unique_ptr<DataFilter>` in `DataSensor` base, ctor/add()
    `make_unique`, `copy()` via `reset(->copy())`, `_resetTo` deep copy
    (aliasing `other->filter()` today, e.g. AmbientLight:86,
    Orientation:209 — same double-free class the noise aliasing had).

1.3 **`NoiseFactory::create` suppression**: covers runtime `_n`
    reassignment leaks (actuators/sensors re-create noise processors
    without freeing the previous one). Give `_n` real ownership
    (`unique_ptr<Noise>`) at the consumer side; delete the suppression
    line in the same commit. FilterFactory call sites get the same
    treatment while in there.

Retires: `leak:NoiseFactory::create`. Effort: one session, mostly
delegable with the binding-table-style agent brief.

## Phase 2 — constructor-owned members in the remaining Data* classes

Identical pattern to the shipped fixes: constructor `new`s owning raw
members, never freed; `copy()`/`_resetTo()` need `.reset(copy())`
adaptation. One commit per class, suppression line deleted in the same
commit:

| Suppression | Members to convert |
|---|---|
| `leak:DataObject::DataObject` | `_physics` (DataPhysicsParameter) |
| `leak:DataComposite::DataComposite` | own members incl. `_physics` chain |
| `leak:DataCameraConfig::DataCameraConfig` | `_orbit`, `_center`, `_offset` (+ siblings in ctor) |
| `leak:DataScreen::DataScreen` | `_camera` (+ audit rest of ctor) |
| `leak:DataScreens::DataScreens` | `_default`, `_cameraConfig`, `_recording`, … |

Watch-outs: `DataObject::_physics` is aliased by shape `_resetTo()`
implementations (`_physics = box->physics()->copy()` after a raw
`delete _physics` — adapt, don't double-own); `DataScreens` is a
`std::vector<DataScreen*>` — the *container* entries are separate from
the ctor members and stay as they are for now.

Retires: 5 suppressions. Effort: one session, agent-delegable per class
with the recipe; gates unchanged.

## Phase 3 — the four "look before you cut" entries

Each needs a short investigation, not just a transform:

3.1 `leak:DataController::add` / `leak:DataController::setModule` —
    allocations tied to controller-module loading (dlopen-adjacent).
    Determine whether the objects are handed to the runtime controller
    layer (aliased) or plainly leaked; fix ownership or split the
    suppression per real owner. Two entries retired if clean.

3.2 `leak:yars::PID::PID` — small util class; likely a trivially
    convertible ctor allocation. Investigate + fix.

3.3 `leak:Composite::init` (physics/bullet) — `btCompoundShape` and
    `MyCompositeMotionState` raw `new`s; Bullet's btRigidBody does NOT
    own motion states/shapes. Store them on the Composite and free in
    its destructor (mind reset/rebuild paths that re-init). Verify with
    the falling_objects + composite corpus configs.

3.4 `leak:Directories::__setup` — util/path setup; likely one-time
    static-lifetime allocations. If truly once-per-process, either fix
    trivially or reclassify as intentional (see Phase 4 rule).

Retires: up to 5 suppressions. Effort: one session.

## Phase 4 — the intentional leak, made explicit

`leak:parseJsonConfig` exists because opening `DataParseElement`s are
deliberately leaked to replay the old SAX handler's audited behaviour
(`Data*::add()` implementations were audited against leaked openings —
some may retain attribute pointers).

Decision for 1.0 (either is acceptable, pick one):
- **(a) Document-and-keep**: annotate the suppression line with the
  rationale and a pointer to JsonParser.cpp's lifetime comment; declare
  it the one intentional entry. Zero risk.
- **(b) Fix properly**: audit whether any `Data*` retains
  `DataParseAttribute*`/element pointers past `add()` (grep suggests
  values are copied out as strings; the audit must confirm), then have
  `loadConfigFile`/`emitElement` own the openings in a
  `vector<unique_ptr<DataParseElement>>` freed after parsing. Medium
  risk; only worth it if the audit comes back clean.

Recommendation: (a) now, (b) opportunistically later.

## Phase 5 (optional, post-1.0) — C++20

No functional driver today. When picked up: bump
`CMAKE_CXX_STANDARD 20`, fix fallout (likely small: designated-init
warnings, `char*` string-literal defines), then adopt incrementally
(`std::format` in logging, ranges in hot loops only if they measure
clean, concepts in DataBinding). Do NOT couple this to the 1.0 gate.

## 1.0 release gate + checklist

Gate: Phases 1–3 merged, Phase 4 decision recorded, CI green.

Checklist for the release commit:
- [ ] `scripts/sanitizer-suppressions.txt` contains only annotated
      intentional entries
- [ ] CMake `VERSION 1.0.0`; `yars --version` reports it
- [ ] Consider bumping the *config schema* version story: add an
      `XmlChangeLog` entry (or freeze it and document that the JSON
      format is versioned by the schema file from now on)
- [ ] README + docs/JSON_Format.md sweep for stale version references
- [ ] Tag `v1.0.0`; verify both CI workflows + sanitize green on the tag

## Suggested order

Phase 1 → 2 → 3 are independent of each other but share the validation
flow; run them as three consecutive feat-branches (or one branch, three
commit groups). Phase 4(a) is a one-line doc change that can ride with
Phase 3. Total: roughly three focused sessions to the 1.0 gate.
