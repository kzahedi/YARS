# Binding-table recipe (JSON Stage 4)

> **STATUS (2026-07-09): all families migrated.** Shapes (pilot),
> sensors (14), actuators (4, with constructor-`_noise` ownership fixes
> and their three `leak:` suppressions retired), logging (9 with
> attributes; DataLogging/DataLoggingModule/DataLoggingConsole have
> none), and roots (DataRobot, DataEnvironment, DataComposite;
> DataRobots and the root description have no own-tag attributes).
> Additionally every unchecked `attribute(x)->` deref now goes through
> `DataParseElement::requiredAttribute` (clean missing-attribute
> errors). This document remains the recipe for any future Data*
> class.

Stage 3 deleted every `createXsd()` method — the XSD-generation half of
each `Data*` class's duplication. What's left is the *attribute-parsing*
half of `Data*::add(DataParseElement*)`: a hand-written block, repeated
almost verbatim across ~90 classes, that pulls named attributes off the
currently-opening tag and assigns them to member fields, with ad hoc
required/default handling (or none at all).

Stage 4 introduces `yars::AttributeBinding` / `yars::applyAttributes`
(`src/yars/configuration/data/DataBinding.h/.cpp`) to replace that block
with a static table, and pilots it on the shape family (`DataBox`,
`DataSphere`, `DataCylinder`, `DataCapsule`, `DataPly`). This document is
the recipe for migrating every remaining family, on whatever future
session picks it up.

## The interface

```cpp
namespace yars {
struct AttributeBinding {
  const char *name;                                           // attribute name in config
  std::function<void(DataNode*, const std::string&)> apply;   // parse+assign
  bool required;
  const char *defaultValue;                                   // nullptr = no default
};
void applyAttributes(DataNode *self, DataParseElement *element,
                      const std::vector<AttributeBinding> &bindings);
}
```

Semantics per binding row, evaluated against the currently-opening
`DataParseElement`:

| attribute state                    | behaviour                                                   |
|-------------------------------------|---------------------------------------------------------------|
| present                             | `apply(self, attribute->value())`                             |
| absent, `required == true`          | throws `std::runtime_error("<node>: missing attribute '<name>'")` |
| absent, `required == false`, has default | `apply(self, defaultValue)`                             |
| absent, `required == false`, no default  | left untouched (matches legacy `DataParseElement::set()`) |

The thrown message is deliberately *not* prefixed with the config file
path — `JsonParser::parseJsonConfig`'s existing `catch (const
std::exception &e)` block already does `errors.push_back(jsonPath + ": "
+ e.what())`, so the message composes into the spec's `file:node:
message` format for free. Don't add a second prefix in `applyAttributes`
or bindings — it would double up.

## Mechanical migration steps (repeat per class)

1. Open the class's `.cpp`. Find the attribute block for its *own*
   opening tag inside `add(DataParseElement*)` — usually the first
   `if(element->opening(YARS_STRING_OBJECT_<X>)) { element->set(...); ...
   }` block, immediately after the `closing(...)` check that resets
   `current`. This is the only part that migrates. **Child-element
   dispatch does not migrate** — `if(element->opening(YARS_STRING_POSE))
   DataPoseFactory::set(...)`, texture/mesh child handling, physics
   sub-object delegation, etc. are a state machine, not a flat attribute
   list; leave them exactly as they are.
2. `#include "DataBinding.h"` under the class's own header include.
3. Add an anonymous-namespace static accessor above the class's
   constructor, one lambda row per attribute in the block you're
   replacing:
   ```cpp
   namespace
   {
   const std::vector<yars::AttributeBinding> &fooAttributeBindings()
   {
     static const std::vector<yars::AttributeBinding> bindings = {
         {YARS_STRING_NAME,
          [](DataNode *self, const std::string &value)
          { static_cast<DataFoo *>(self)->setName(value); },
          /*required=*/false, /*defaultValue=*/nullptr},
         // ...
     };
     return bindings;
   }
   } // namespace
   ```
   Each lambda takes `DataNode *self`, `static_cast`s to the concrete
   class, and calls a **public setter**. If the field has no public
   setter yet (common — legacy code often set private members directly
   from inside `add()`, which a free-standing lambda can't do), add one.
   `DataPly` needed three (`setFilename`, `setConvex`, `setSoft`) — adding
   them is in scope for the migration, not scope creep.
4. Replace the block's body with one line:
   `yars::applyAttributes(this, element, fooAttributeBindings());`
5. Preserve exact semantics: if the legacy code used `element->set(name,
   _field)` (a plain string/bool/double/int `set()` overload, no
   required-check, no default — the common case), the binding row is
   `required=false, defaultValue=nullptr`. Only mark a row `required=true`
   or give it a `defaultValue` if the legacy code already had that
   behaviour hand-rolled (rare in this codebase today) — **don't invent
   new required/default semantics as part of a mechanical migration**;
   that's a behaviour change and belongs in its own commit/PR with its
   own review, not folded into the table-conversion diff.
6. Build, run the full `yars_tests` suite, then both regression gates
   (below). Commit **one class per commit** — small diffs, easy bisect,
   easy revert if a gate goes red.

## Worked example: `DataBox` before/after

Before (`DataBox::add`, attribute half only):
```cpp
if(element->opening(YARS_STRING_OBJECT_BOX))
{
  element->set(YARS_STRING_NAME,      _name);
  element->set(YARS_STRING_VISUALISE, _visualise);
}
```

After:
```cpp
namespace
{
const std::vector<yars::AttributeBinding> &boxAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataBox *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_VISUALISE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataBox *>(self)->setVisualise(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace
```
```cpp
if(element->opening(YARS_STRING_OBJECT_BOX))
{
  yars::applyAttributes(this, element, boxAttributeBindings());
}
```

Note the raw LOC count for a 2-attribute class goes *up* (a ~15-line
table vs. a 2-line block) — the win isn't per-class line count on small
classes, it's (a) a single, tested, uniform place that owns
required/default/error semantics instead of each class hand-rolling its
own (or, for most classes today, hand-rolling *none*), and (b) the win
compounds on classes with more attributes: `DataPly` replaced a 5-line
block with a 5-row table for the same ~+20 LOC overhead, and any class
with 8+ attributes (several sensor/actuator classes qualify) will show a
net LOC *reduction* once the constant per-class table-scaffolding cost is
amortised. Don't chase LOC as the metric for small classes; the metric
that matters is "one code path owns attribute semantics," which is what
makes the next step — enforcing `required` attributes that today parse
silently-wrong or crash downstream — a one-line change instead of a
class-by-class audit.

## Priority order for remaining families

Recommended order, each family an **independent future task** using this
exact recipe under the same regression gate — explicitly OK to schedule
each as its own session:

1. **Sensors** (`DataGenericProximitySensor`, `DataGenericPositionSensor`,
   `DataGenericVelocitySensor`, `DataGenericOrientationSensor`,
   `DataGenericAmbientLightSensor`, `DataGenericLightDependentResistorSensor`,
   `DataGenericBinaryContactSensor`, `DataGenericCameraSensor`,
   `DataGenericDeflectionSensor`, `DataGenericFeedbackSensor`,
   `DataGenericInternalEnergySensor`, `DataGenericSignalSensor`,
   `DataGenericActuatorSensor`, `DataMuscleSensor`) — largest family,
   biggest attribute counts, biggest LOC win.
2. **Actuators** (`DataHingeActuator`, `DataSliderActuator`,
   `DataMuscleActuator`, `DataGenericActuator`).
3. **Logging** (`DataLogging`, `DataLoggingActuator`, `DataLoggingSensor`,
   `DataLoggingObject`, `DataLoggingController`, `DataLoggingModule`,
   `DataLoggingCSV`, `DataLoggingConsole`, `DataLoggingFile`,
   `DataLoggingGnuplot`, `DataLoggingBlender`, `DataLoggingSelforg`).
4. **Robot/environment roots** (`DataRobot`, `DataRobots`,
   `DataEnvironment`, `DataComposite`, `DataRobotSimulationDescription`) —
   do these last: they're the classes other migrations' regression gates
   already exercise indirectly, so bugs here are highest-blast-radius and
   benefit from every earlier family already being proven safe under the
   same table mechanism.

## Suppression-retirement step

`scripts/sanitizer-suppressions.txt` carries `leak:Data*` entries for
classes whose constructors allocate owning raw members that are never
freed (tracked as legacy, not fixed in place, per Stage 3's harvest
commit). **This is a distinct step from the attribute-table migration
and only applies when it applies**: check the suppressions file for an
entry matching the class you're migrating.

- If the family you're migrating has no entry (true for all five shape
  classes in this pilot — `DataBox`/`DataSphere`/`DataCylinder`/
  `DataCapsule`/`DataPly` were never listed), there's nothing to retire;
  say so in the migration's commit/PR and move on.
- If it does have an entry (e.g. a future sensor/actuator migration hits
  `DataGenericProximitySensor`, `DataHingeActuator`,
  `DataSliderActuator`, or `DataMuscleActuator`, all currently listed):
  1. While you're already in the constructor for the attribute-table
     work, check whether it has raw-pointer members allocated with `new`
     and never deleted (that's what earned the suppression).
  2. Give the member real ownership — `std::unique_ptr` if exclusively
     owned and never aliased, or an explicit paired `delete` in the
     destructor if a raw pointer must stay (e.g. for an existing
     `copy()`/`_resetTo()` aliasing pattern that assumes raw pointers).
  3. Delete that class's `leak:Data*` line from
     `scripts/sanitizer-suppressions.txt` **in the same commit** as the
     ownership fix.
  4. Run `scripts/sanitize-corpus.sh <sanitized-build-dir>` (or trigger
     the CI `sanitize` job) and confirm it stays green — a suppression
     line outliving its leak is a silent regression hole: the *next*
     unrelated leak in that constructor would be masked by the same
     line, not caught.
  5. If the ownership fix is risky/nontrivial enough to want its own
     review cycle, it's fine to leave the suppression in place and do
     the ownership fix as a following, separate commit — just don't
     silently skip the retirement once ownership *is* fixed; a
     suppression with no matching leak is dead weight the sanitize job
     should catch you leaving behind.

## Regression gate (run after every class, every family)

```bash
cmake --build build -j 6
./build/bin/yars_tests                                  # must be 100% green

cd build
rm -f braitenberg-*.csv hexapod-*.csv
./bin/yars --iterations 2000 --xml ../xml/braitenberg_logging.json --nogui
diff braitenberg-*.csv ../xml/reference_logfile.macos-arm64.csv   # must be empty

./bin/yars --iterations 2000 --xml ../xml/hexapod_logging.json --nogui
diff hexapod-*.csv ../xml/reference_logfile_hexapod.macos-arm64.csv  # must be empty
```

On Linux, diff against `xml/reference_logfile*.linux-x86_64.csv`
instead. Both platform references must stay bit-exact; CI enforces both.

Additionally, once a full family is migrated (not necessarily after each
individual class), re-run `scripts/sanitize-corpus.sh` against a
sanitized build (`-DYARS_ENABLE_SANITIZERS=ON -DYARS_BUILD_TESTS=OFF`) —
13/13 configs must still pass, and any suppression retired in that
family's commits must not resurrect a leak.
