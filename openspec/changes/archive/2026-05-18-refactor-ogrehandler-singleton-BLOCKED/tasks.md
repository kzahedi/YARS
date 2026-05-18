# Tasks

## 1. Pre-flight

- [x] 1.1 **Result: BLOCKED.** Inspected Ogre 14's
      `OgreSingleton.h:91-93`:
      ```cpp
      Singleton(void)
      {
          OgreAssert(!msSingleton, "There can be only one singleton");
          msSingleton = static_cast<T*>(this);
      }
      ```
      `Ogre::Root` inherits from `Singleton<Root>` (see
      `OgreRoot.h:36` + `OgreRoot.cpp:84`:
      `template<> Root* Singleton<Root>::msSingleton = 0;`).
      Constructing a second `Ogre::Root` in the same process aborts.
      The "one Root per process" constraint is enforced by the
      framework itself, not by YARS.
- [x] 1.2 Catalogued `OgreHandler::instance()` callers anyway:
      `YarsViewModel`, `SdlWindow`, plus the wiring around
      capture / shaders. Listed for reference if anyone re-opens
      this with reduced scope.

## 2. Decision

- [x] 2.1 The main proposal motivation — unblocking
      Vulkan-side-by-side and per-test setup/teardown of an Ogre
      stack — is **unreachable while we use Ogre at all**. Multi-Root
      requires either an Ogre fork that drops the framework-level
      singleton assertion, or a process-isolation approach
      (separate yars subprocesses sharing data via IPC).
- [x] 2.2 The secondary motivation — replacing the YARS-side
      `OgreHandler::instance()` accessor with explicit unique_ptr
      ownership inside `YarsViewModel` — is achievable but
      cosmetic. It rearranges deck chairs around an immovable
      Ogre constraint. Deferred until there's a concrete driver
      (e.g., the openspec/specs work for headless-test scenarios
      hits a real ownership clash).
- [x] 2.3 Close this change as **BLOCKED**. Archive under that
      label.

## 3. Documentation

- [x] 3.1 Cross-reference the constraint in
      `docs/planning/v0.8.7-open-points.md`. Update the OgreHandler
      entry from "could be done" to "framework-blocked".
