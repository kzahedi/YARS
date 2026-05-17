# Tasks

## 1. Pre-flight

- [ ] 1.1 Confirm Ogre 14 actually supports more than one `Root`
      per process. Check Ogre's own samples and the GL3+ render
      system source. If it does not, write up the finding and
      close this change as "blocked by Ogre constraint".
- [ ] 1.2 `grep -rn "OgreHandler::instance" src/` — catalogue
      every caller. Expect at minimum: `YarsViewModel`,
      `SdlWindow`, `YarsViewControl`, capture wiring,
      `ShaderManager`.

## 2. API change

- [ ] 2.1 `OgreHandler.h`: drop `static OgreHandler *instance()`;
      make the constructor public. Drop the `static OgreHandler *_me`
      member and its definition in `OgreHandler.cpp`.
- [ ] 2.2 `OgreHandler.h`: add a non-copyable, non-movable
      annotation (`= delete` on copy/move ctors) so accidental
      duplicates fail at compile time, not at runtime.

## 3. Plumb ownership through `YarsViewModel`

- [ ] 3.1 `YarsViewModel.h`: change `OgreHandler *_ogreHandler` to
      `std::unique_ptr<OgreHandler> _ogreHandler`.
- [ ] 3.2 `YarsViewModel.cpp` constructor: replace
      `_ogreHandler = OgreHandler::instance()` with
      `_ogreHandler = std::make_unique<OgreHandler>()`.
- [ ] 3.3 All callers in `YarsViewModel` that did
      `_ogreHandler->...` still work (unique_ptr operator->).
      Verify the `if (_ogreHandler != NULL)` guards still mean
      what they did.

## 4. Plumb ownership through `SdlWindow`

- [ ] 4.1 `SdlWindow` constructor: add an `OgreHandler*`
      parameter. Drop the internal `OgreHandler::instance()`
      call.
- [ ] 4.2 `YarsViewModel::__createWindow` and
      `YarsViewModel::createNewWindow`: pass
      `_ogreHandler.get()` into the new `SdlWindow`.

## 5. Other callers

- [ ] 5.1 For each caller catalogued in 1.2 that isn't covered by
      3 / 4 above, decide: pass via constructor / setter, or take
      a const reference. Avoid global state.

## 6. Validation

- [ ] 6.1 macOS build + smoke (braitenberg_logging.xml --nogui
      and GUI 200 iter).
- [ ] 6.2 Linux CI green on the audit corpus + capture step.
- [ ] 6.3 Cross off the OgreHandler item in
      `docs/planning/v0.8.7-open-points.md`.
