# Refactor OgreHandler away from singleton pattern

## Why

`OgreHandler` is the last surviving Meyers-style singleton in YARS
after the v0.8.7 unique_ptr sweep:

```cpp
class OgreHandler {
  public:
    static OgreHandler *instance();
    ...
};
```

It was excluded from the migration because `YarsViewModel` and
`SdlWindow` both reach for `OgreHandler::instance()` directly. The
unique_ptr pattern needs a single owner; today no class owns Ogre.

This is fine today because we only ever create one Ogre `Root`. It
becomes a problem when:

1. **Vulkan side-by-side** — the in-tree
   `src/yars/view/vulkan/` work-in-progress wants to spin up a
   second renderer for comparison. Two singletons collide.
2. **Multi-window / multi-context** — if a future config wants the
   physics view + a top-down map in separate Ogre contexts, the
   singleton blocks it.
3. **Unit tests** — anything that wants to construct an Ogre stack,
   tear it down, and construct a new one in the same process (e.g.,
   testing renderer init / failure modes) can't, because the
   singleton holds onto its state.

## What changes

1. **Make `OgreHandler` constructible.** Remove the `instance()`
   accessor; expose a public constructor.
2. **`YarsViewModel` owns the OgreHandler.** Currently:
   ```cpp
   _ogreHandler = OgreHandler::instance();
   ```
   Becomes:
   ```cpp
   _ogreHandler = std::make_unique<OgreHandler>();
   ```
   Stored as `std::unique_ptr<OgreHandler>`.
3. **Pass it to `SdlWindow`** explicitly. Today `SdlWindow` reaches
   for `OgreHandler::instance()` in its constructor. Change the
   `SdlWindow` ctor to take an `OgreHandler*` parameter, plumb it
   through from `YarsViewModel::createNewWindow` /
   `__createWindow`.
4. **Audit other callers.** `grep` for `OgreHandler::instance()`
   in `src/` — there may be straggler references in shader
   manager, capture pipeline, etc.

## Non-goals

- Supporting multiple Ogre contexts in the same process. This
  proposal only unblocks that path; the change is "one owner
  instead of zero owners".
- Touching the Vulkan renderer's own structure. Separate concern.

## Open questions

- Does Ogre 14 actually permit constructing two `Ogre::Root`
  instances in the same process? Some renderers (notably GL3+
  with the GLX context-share assumption) historically did not.
  If not, the singleton is enforcing a real underlying constraint
  and removing it just shifts the trap. Audit this before
  committing to the change.
