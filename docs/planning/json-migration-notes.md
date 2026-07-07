# JSON Stage 1 — XML parser event-stream contract (characterization notes)

Read-only characterization of the existing XSD/SAX parsing pipeline. All
claims below are cited by `file:line` against the working tree at
`/Volumes/Eregion/projects/yars-json`. A JSON-driven replayer must reproduce
this event contract exactly (same DataParseElement objects, same order, same
lifetime rules) so that all `Data*::add(DataParseElement*)` state machines
(80+ factory/data classes) work completely unchanged.

## 1. How opening/closing `DataParseElement` events are constructed

File: `src/yars/configuration/xsd/parser/YarsXSDSaxHandler.cpp`

- `startElement(uri, localname, qname, attrs)` (handler signature at
  `YarsXSDSaxHandler.h:27-31`; body at `YarsXSDSaxHandler.cpp:16-49`):
  - Tag name source is **`localname`**, not `qname` — see
    `YarsXSDSaxHandler.cpp:22` (`message = XMLString::transcode(localname)`)
    and `:27` (`element->setName(message)`). `qname` and `uri` are
    transcoded (`:24-25`) but never used for the element name — they are
    transcoded only so the corresponding `XMLString::release` calls balance;
    functionally dead values (`m_uri`, `m_localname`, `m_qname` are computed
    but never read except to free them, `:44-47`).
  - Namespaces ARE turned on in the reader
    (`XMLUni::fgSAX2CoreNameSpaces = true`,
    `YarsXSDSaxParser.cpp:51`/`:177`), but namespace **prefixes** are off
    (`fgSAX2CoreNameSpacePrefixes = false`, `YarsXSDSaxParser.cpp:55`/`:181`).
    Since the schema and all XML documents in this codebase are
    non-namespaced, `uri` is effectively always empty; only `localname`
    matters. A JSON replayer only needs a plain tag-name string — no
    namespace/prefix handling required.
  - Opening element construction: `new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_OPENING)`
    (`YarsXSDSaxHandler.cpp:26`), type constant `2001`
    (`DataParseElement.h:10`).
  - Attributes are attached only if `attrs.getLength() > 0`
    (`YarsXSDSaxHandler.cpp:29`). For each attribute a `DataParseAttribute`
    is created (`:33`) with:
    - name = `attrs.getQName(i)` transcoded to `std::string`
      (`YarsXSDSaxHandler.cpp:34,39`) — note this is the **qname** for
      attributes (not localname as for elements). Since there's no
      namespace prefixing in practice, qname == localname for attributes
      too, in this codebase.
    - value = `attrs.getValue(i)` transcoded to `std::string`
      (`:38,40`).
    - `attrs.getURI(i)` and `attrs.getLocalName(i)` and `attrs.getType(i)`
      are transcoded (`:35-37`) but **never used** — dead code, same
      pattern as the element-name dead transcodes above.
    - Attribute is appended via `element->add(a)` →
      `push_back(attribute)` (`DataParseElement.cpp:32-35`); `DataParseElement`
      itself *is* a `std::vector<DataParseAttribute*>`
      (`DataParseElement.h:20`).
    - All attribute values are stored purely as strings
      (`DataParseAttribute::_value`, `DataParseAttribute.h:91`); typed
      accessors (`intValue()`, `realValue()`, `boolValue()`,
      `unsignedlongValue()`) do lazy conversion via `atoi`/`atof`/`"true"`
      comparison at read time with **no error handling**
      (`DataParseAttribute.cpp:37-55`). A JSON reader must therefore also
      produce all attribute values as strings (or something that supports
      identical stringly-typed conversions) if it wants to reuse
      `DataParseElement::set(...)` (`DataParseElement.cpp:73-111`), which
      calls these same converters.
  - Finished opening element is handed to the spec:
    `_spec->add(element)` (`YarsXSDSaxHandler.cpp:48`) — see §2.
  - There is **no separate "closing" constructor** — the *type* is what
    distinguishes opening vs. closing (`YARS_DATA_PARSE_ELEMENT_TYPE_OPENING`
    = 2001 vs `YARS_DATA_PARSE_ELEMENT_TYPE_CLOSING` = 2002,
    `DataParseElement.h:10-11`). Both are the same class, constructed with a
    different `int type` ctor arg (`DataParseElement.h:32`,
    `DataParseElement.cpp:3-7`).

- `endElement(uri, localname, qname)` (`YarsXSDSaxHandler.h:33-36`; body
  `YarsXSDSaxHandler.cpp:51-62`):
  - Only `localname` is used (`:53,56`); `uri`/`qname` params are unnamed in
    the signature (`YarsXSDSaxHandler.h:34,36` — the `uri` and `qname`
    parameters are anonymous, confirming they're intentionally unused).
  - Closing element: `new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_CLOSING)`
    (`:57`), name set from `localname` (`:58`), **no attributes are ever
    attached to a closing element** — `endElement` has no `Attributes`
    parameter to draw from, and nothing calls `add()` on it.

## 2. Dispatch: `_spec->add(element)` and per-class matching

- `_spec` is a `DataRobotSimulationDescription*`
  (`YarsXSDSaxHandler.h:67`), set once per document in `startDocument()`:
  `_spec = Data::instance()->newSpecification();` (`YarsXSDSaxHandler.cpp:66`,
  declared `YarsXSDSaxHandler.h:38`).
- `DataRobotSimulationDescription::add(DataParseElement*)`
  (`DataRobotSimulationDescription.cpp:112-123`):
  ```
  if(current == NULL) current = this;
  if(current == this) __getChild(element);
  else current->add(element);
  ```
  `current` is a `DataNode*` member (inherited; set in the ctor to `this`,
  `DataRobotSimulationDescription.cpp:36`). This is a simple **tree-walk
  dispatcher**: every event is routed to whichever `DataNode` subtree is
  "current" at that moment, and every intermediate node's own `add()`
  recurses further down until reaching whichever concrete `Data*` class
  actually owns the currently-open tag. There is no stack structure
  visible at this call site — instead each concrete class flips `current`
  to itself on its own opening tag and flips it back to `parent` on its own
  closing tag (see `DataBox::add`, `DataBox.cpp:42-45`: `if
  (element->closing(YARS_STRING_OBJECT_BOX)) current = parent;`). This
  means the "stack" is implicit and distributed: each node keeps a
  `parent` pointer and reassigns the ancestor-shared `current` variable
  as elements open/close. A JSON replayer must feed events in the same
  strict open/close nesting order the XML SAX stream would produce, because
  this dispatch mechanism has no lookahead or backtracking — it purely
  reacts to the sequential event stream.
- Root-level element matching (`__getChild`,
  `DataRobotSimulationDescription.cpp:125-252`) is a **chain of
  `if(element->opening(TAG))` checks** — plain string compares, no dispatch
  table, no `switch`. Order in the file is the only precedence.
- `DataParseElement::opening(string name)` /
  `closing(string name)` (`DataParseElement.cpp:53-71`):
  ```
  bool opening(string name) { return _name == name && _type == OPENING; }
  bool closing(string name) { return _name == name && _type == CLOSING; }
  ```
  Exact, case-sensitive `std::string::operator==` comparison. There is a
  `char*` overload too (`opening(char*)`/`closing(char*)`,
  `DataParseElement.cpp:58-61,68-71`) that just wraps the string in
  `std::string(name)` — used everywhere via `YARS_STRING_*` `(char*)"..."`
  macros (e.g. `DataBox.cpp:11-23`). No case-folding, no trimming, no
  namespace-qualification — the tag name is compared literally byte-for-byte
  against the macro string literal.
- Representative leaf class `DataBox::add(DataParseElement*)`
  (`DataBox.cpp:40-86`) follows the identical pattern: a flat sequence of
  `if(element->opening(TAG))` / `if(element->closing(TAG))` checks, each
  either mutating local state directly (`element->set(...)`,
  `element->attribute(...)->value()`) or constructing a child `Data*`
  object and reassigning `current` to it (e.g. opening `physics` sets
  `current = _physics` at `DataBox.cpp:60-64`; opening a mesh-visualisation
  tag allocates a new `DataMeshVisualisation`, appends it, and reassigns
  `current` to it, `DataBox.cpp:79-85`). Closing `box` resets
  `current = parent` (`DataBox.cpp:42-45`) — note this specific `if` has
  **no `return`/`else`** guarding it from the rest of the function; the
  closing check runs first and other `opening(...)` checks below still
  execute on the same event object even after `current` has already been
  reset (harmless here since a closing-box element never simultaneously
  matches any of the opening checks below it, but it shows the dispatch
  style is "run every check every time," not "match once and stop").

## 3. Lifetime rules

- **Opening elements**: allocated with `new` in `startElement`
  (`YarsXSDSaxHandler.cpp:26`), handed to `_spec->add(element)`
  (`:48`), and **never explicitly deleted by the handler**. Nothing in
  `YarsXSDSaxHandler.cpp` frees an opening `DataParseElement`. Whether any
  downstream `Data*::add()` deletes it was not found in the classes read
  for this task (`DataRobotSimulationDescription::add`,
  `DataBox::add` — neither deletes the `element` pointer they're given).
  **Conclusion: opening `DataParseElement` instances are leaked** for the
  lifetime characterization purposes (confirmed no matching `delete
  element` anywhere in the read files). A JSON replayer that constructs
  its own `DataParseElement` objects to feed into the *unchanged*
  `Data*::add()` machinery should preserve this leak rather than
  "fixing" it, to avoid double-free/use-after-free if any `Data*::add()`
  elsewhere in the tree (not read here) retains the pointer past the call.
- **Closing elements**: allocated, immediately passed to `_spec->add(element)`,
  then **immediately `delete`d in the same function**, before
  `XMLString::release`:
  ```cpp
  DataParseElement *element = new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_CLOSING);
  element->setName(message);
  _spec->add(element);
  delete element;                 // YarsXSDSaxHandler.cpp:60
  XMLString::release(&message);
  ```
  (`YarsXSDSaxHandler.cpp:57-61`). This **confirms the known fact stated in
  the task**: `endElement` does add-then-delete synchronously, so every
  `Data*::add()` invoked for a closing tag must finish all its own work
  (reading `element->name()`, checking `closing(TAG)`, reassigning
  `current`) synchronously within the `_spec->add(element)` call — nothing
  downstream may store the closing element pointer and use it later. A
  JSON replayer must likewise delete each closing `DataParseElement`
  synchronously right after the `add()` call returns (or otherwise avoid
  handing out a pointer that outlives that single dispatch), matching
  exactly this pattern.
- `DataParseElement::~DataParseElement()` (`DataParseElement.cpp:9-15`)
  deletes every attribute it owns (`for i in size(): delete at(i)`) — so
  deleting a closing element is cheap (it never has attributes, per §1) but
  deleting an opening element (if anyone ever does) would free its
  attributes too.

## 4. Events beyond open/close

`YarsXSDSaxHandler` overrides (`YarsXSDSaxHandler.h:27-43`):
`startElement`, `endElement`, `startDocument`, `endDocument`, `error`,
`fatalError`, `warning`. It extends `xercesc::DefaultHandler`
(`YarsXSDSaxHandler.h:22`), which also has a virtual `characters(...)`
callback — **`YarsXSDSaxHandler` does NOT override `characters()`**, so
text content between tags is silently discarded via the base-class no-op
implementation. **Confirmed: no `characters()` handling exists** — grep
for `characters` in `YarsXSDSaxHandler.{h,cpp}` returns nothing.
This means the XML format here is purely attribute-driven; there is no
inline text-node payload for any element in the schema that the parser
actually looks at. A JSON replayer does not need a "characters" event
equivalent.

- `startDocument()` (`YarsXSDSaxHandler.cpp:64-67`) is the **only place**
  `_spec` gets initialized — `_spec = Data::instance()->newSpecification();`.
  This must run exactly once per document, before the first `startElement`.
- `endDocument()` (`YarsXSDSaxHandler.cpp:69-72`) is a no-op ("noting" —
  literal comment typo in the source).
- `error`/`fatalError`/`warning` (`YarsXSDSaxHandler.cpp:74-102`) push
  formatted strings (`"Error: <msg> at line: <n>"` etc.) into
  `_errors`/`_fatals`/`_warnings` vectors — **they do not mutate `_spec`
  or throw**; they just accumulate messages for the caller
  (`YarsXSDSaxParser::read`) to inspect after `parser->parse(...)` returns
  (see §6). Schema-validation errors surface via `error()`/`fatalError()`,
  not via C++ exceptions, in the common case (exceptions are also caught
  separately for XML well-formedness failures — see §6).

## 5. Where the XML path is chosen in `YarsConfiguration`

File: `src/yars/configuration/YarsConfiguration.cpp`

- `YarsConfiguration::__readXmlFiles()` (`:216-240`) is the single call
  site that instantiates the SAX parser:
  ```cpp
  Data::instance()->clear();
  string xml = getXml();                                   // :219
  auto parser = std::make_unique<YarsXSDSaxParser>();       // :221
  parser->read(xml);                                        // :223
  if (parser->errors() > 0) { ... exit(-1); }               // :224-233
  ```
  **This is the exact insertion point for a `.json` branch**: after
  `string xml = getXml();` (`:219`), branch on file extension (or content)
  and either call `parser->read(xml)` (existing XML path, `:221-223`) or a
  new JSON-driving equivalent that ultimately performs the same sequence
  of `_spec->add(element)` calls that `YarsXSDSaxHandler` performs today.
  `getXml()` (accessor declared via `ConfigurationContainer`, not read in
  this pass) returns the path string exactly as given on the command line
  — including the literal `"-"` sentinel for stdin.
  - The parser is handed a **file path string** — `parser->read(filename)`
    signature (`YarsXSDSaxParser.h:28`), passed straight to
    `parser->parse(filename.c_str())` (`YarsXSDSaxParser.cpp:84`) for the
    Xerces `SAX2XMLReader`, which opens/reads the file itself. Nothing in
    YARS pre-reads the file into a buffer for the normal (file) path.
  - The **stdin `"-"` mode**: `YarsXSDSaxParser::read` special-cases
    `filename == "-"` (`YarsXSDSaxParser.cpp:77-81`):
    ```cpp
    if(filename == "-") { StdInInputSource src; parser->parse(src); }
    else                { parser->parse(filename.c_str()); }
    ```
    A JSON replayer needs the equivalent: if the path is `"-"`, read JSON
    from stdin instead of opening a file.
  - `YarsConfiguration::__validateXmlPath()` (`:286-...`, reads
    `string xml = getXml()` at `:288`) validates existence via
    `_directories->doesFileExist(xml)` **only when `xml != "-"`**
    (`:295-301`) — i.e. stdin mode deliberately bypasses the file-exists
    check. Any JSON path-selection logic inserted before/alongside this
    must preserve the same `"-"` bypass for a `.json` stdin mode if that's
    desired, and should be inserted consistently in both
    `__validateXmlPath()` and `__readXmlFiles()` (there are two independent
    `string xml = getXml();` call sites: `:219` and `:288`).

## 6. Validation side effects

- Schema validation is Xerces SAX2 core validation
  (`XMLUni::fgSAX2CoreValidation = true`, `YarsXSDSaxParser.cpp:48`/`:175`),
  loaded from an in-memory generated XSD (`YarsXSDGenerator`, `:37-41` /
  `:164-170`), not a static file. Validation is purely rejecting: nowhere in
  `YarsXSDSaxHandler`/`YarsXSDSaxParser` is there any code that injects a
  schema default value into a `DataParseElement`/`DataParseAttribute`
  when an attribute is omitted from the XML — the schema is used only to
  flag structural/type errors via `error()`/`fatalError()`/`warning()`
  callbacks (confirmed: those three overrides only push message strings,
  `YarsXSDSaxHandler.cpp:74-102`; no `_spec` or `element` mutation
  anywhere in them). Any "default when absent" behavior in this codebase
  happens later, inside individual `Data*` classes' own C++ default field
  initializers — not in the parser layer.
- Two failure channels, both surfaced in `YarsXSDSaxParser::read`
  (`YarsXSDSaxParser.cpp:23-145`):
  1. **C++ exceptions** (`XMLException`, `SAXParseException`) thrown out of
     `parser->parse(...)` (well-formedness failures, I/O errors) — caught
     at `:87-98` and `:99-110`; on catch, the message is printed to
     `cout`, the parser/handler are cleaned up, `XMLPlatformUtils::Terminate()`
     is attempted, and `read()` returns `false` immediately (accumulated
     `_errors`/`_warnings`/`_fatals` from the handler are **not** copied
     back to the `YarsXSDSaxParser` members in this exception path — they
     are only copied at `:117-119`, which is skipped when an exception is
     caught early).
  2. **Non-throwing schema validation errors**, accumulated in the handler
     via `error()`/`fatalError()`/`warning()` and copied to the parser's own
     `_errors`/`_fatals`/`_warnings` at `YarsXSDSaxParser.cpp:117-119` after
     `parser->parse()` returns normally. `read()` returns `false` iff
     `_fatals.size() > 0 || _errors.size() > 0` (`:140-143`) — warnings
     alone do not fail the parse.
  - Caller-side (`YarsConfiguration::__readXmlFiles`,
    `YarsConfiguration.cpp:224-233`): checks `parser->errors() > 0`
    (`YarsXSDSaxParser::errors()` = `_fatals.size() + _errors.size()`,
    `YarsXSDSaxParser.cpp:316-319`), prints all warnings/errors/fatals, and
    calls `exit(-1)` (`YarsConfiguration.cpp:232`) — hard process exit, not
    an exception bubbling further up.
  - A JSON reader must fail comparably: it should be capable of (a)
    throwing/failing hard on malformed JSON (structural parse failure,
    analogous to the `XMLException`/`SAXParseException` catch blocks) and
    (b) accumulating semantic/schema-shaped validation errors into
    equivalent `_errors`/`_fatals`/`_warnings` lists so the exact same
    `if (parser->errors() > 0) { ...; exit(-1); }` block in
    `YarsConfiguration::__readXmlFiles` keeps working unmodified (or with
    minimal changes) against the new reader.
  - Additionally, `DataRobotSimulationDescription::__getChild` itself calls
    `exit(-1)` directly for XML **version** mismatches on the root
    `rosiml` opening tag (`DataRobotSimulationDescription.cpp:184-195`,
    `:196-209`) — this is a third, semantically separate failure path that
    is *not* routed through the SAX error handler at all; it fires the
    moment the opening `rosiml` element (with its `version` attribute) is
    dispatched, i.e. mid-stream, not at end-of-parse. A JSON replayer must
    still emit an opening `rosiml`-equivalent element carrying a `version`
    attribute, in the same position (very first element), for this check
    to keep working unchanged.

## 7. Surprises / gotchas for the implementer

- **Root element name is `rosiml`**, not e.g. `yars` or `simulation` —
  macro `YARS_STRING_ROSIML = "rosiml"` (`DataRobotSimulationDescription.h:27`),
  used both for XSD root generation
  (`DataRobotSimulationDescription.cpp:344`, `_root = new
  XsdSequence(YARS_STRING_ROSIML)`) and for the version-check dispatch in
  `__getChild` (`:176`). The generated schema file is even named
  `rosiml.xsd` (`YarsConfiguration.cpp:166`). Any JSON equivalent likely
  wants a top-level `"rosiml"`-named object/wrapper (or at minimum, the
  replayer must synthesize a `DataParseElement` opening event named
  `"rosiml"` with a `version` attribute as the very first event, and a
  matching closing `"rosiml"` event as the very last).
- **Tag-name matching is by `localname`, not `qname`**, and is a plain
  case-sensitive `std::string ==`. No trimming/normalization occurs
  anywhere in the read files. JSON keys must match these XML tag-name
  strings byte-for-byte (same macros, e.g. `"box"`, `"dimension"`,
  `"pose"`, `"texture"`, `"first"`/`"second"`/... for the 6-texture box
  faces, `DataBox.cpp:14-19`) if the replayer wants to drive the unchanged
  `Data*::add()` code.
- **All attribute values are strings end-to-end.** Even numeric/boolean
  attributes are stored as `std::string` in `DataParseAttribute::_value`
  and only converted on demand via `atoi`/`atof`/`== "true"` with zero
  error handling (`DataParseAttribute.cpp:37-55`). A JSON reader that
  hands numeric/boolean JSON values straight through as C++ `double`/`bool`
  would break `DataParseElement::set(...)` unless it first stringifies
  them (e.g. JSON number `3.5` → `DataParseAttribute::setValue("3.5")`)
  exactly as the XML attribute string would have looked, matching what
  `atof`/`atoi` expect (locale-independent `atof`, no thousands separators,
  `"true"`/anything-else for bool).
- **No stack — a single mutable `current` pointer** threads through the
  whole `DataNode` hierarchy (`DataRobotSimulationDescription.cpp:36`,
  `:112-123`; `DataBox.cpp:42-45,60-64,79-85`). This means the replayer
  must emit strictly well-nested open/close pairs in depth-first order —
  exactly like a SAX stream — because there's no tolerance for
  out-of-order or overlapping events; the mechanism has no way to detect
  or recover from malformed nesting (it will silently misattribute
  elements to the wrong `current` node instead of erroring).
- **Dead/no-op transcodes are pervasive**: `uri`, `qname`, `m_localname`
  local variables in `startElement` (`YarsXSDSaxHandler.cpp:23-25`), and
  `a_uri`/`a_local`/`a_type` per-attribute (`:35-37`) are computed and
  released but never read. Don't assume every transcoded field is
  semantically meaningful — check actual usage.
- **Opening `DataParseElement`s appear to leak** (see §3) — this is
  existing behavior, not something Stage 1 should "fix" as a side effect;
  changing it could change memory/ownership assumptions in code not
  audited here (all 80+ `Data*::add()` implementations were not
  individually checked for whether any of them takes ownership/deletes the
  opening element it's handed).
- **`DataBox::add`'s closing-tag branch has no `return`** (`DataBox.cpp:42-45`),
  unlike most other closing-tag handling seen in
  `DataRobotSimulationDescription::__getChild` (some branches `return`
  after handling an opening tag, e.g. `:132,140,156,166,173,222`; others,
  like `traces`/`logging`/`rosiml`-closing, fall through to the bottom of
  the function without returning, `:230,237,251`). This inconsistency is
  pre-existing in the XML-driven code; a JSON replay implementer should not
  be surprised if some classes exhibit near-miss double-matching for a
  single event — but since dispatch is by exact `(type, name)` match, this
  is harmless in practice (a "closing box" event can't also be "opening
  pose", etc.).
- **`errors` from an early-thrown exception during `parser->parse()` are
  not copied back to `_errors`/`_fatals`/`_warnings`** (see §6) — only the
  normal-return path copies handler state to the parser object
  (`YarsXSDSaxParser.cpp:117-119` / `:240-242`). If a JSON reader wants
  parity, ensure both exception-style hard failures *and* validation-style
  soft failures are distinguishable to the caller the same way (`read()`
  returning `false` either way, but only soft failures populate the
  message lists that `YarsConfiguration::__readXmlFiles` prints).

## 8. JSON validation gap (Stage 1 known limitation — needs Stage 1→2 sign-off)

Today, malformed XML is rejected by Xerces XSD validation (§6) **before**
any `Data*::add(DataParseElement*)` body runs. That validation is what
makes it safe for 80+ `Data*::add()` implementations to dereference a
required attribute unchecked — e.g.
`_physics->setMass(element->attribute(YARS_STRING_KG)->realValue())`
(`DataBox.cpp:67`) assumes `attribute("kg")` is never `NULL`, because the
schema already guaranteed `kg` was present on `<mass>` before this code
ever ran.

`JsonParser` (`src/yars/configuration/json/JsonParser.{h,cpp}`) has **no
equivalent schema validation**. It catches JSON-structural problems it can
see — invalid JSON syntax, a missing top-level `rosiml` key, a `#children`
entry missing its `#tag` field — and reports them cleanly via the
`errors` out-parameter, matching `YarsXSDSaxParser::errors()`'s contract
closely enough that `YarsConfiguration::__readXmlFiles` prints and
`exit(-1)`s the same way for both formats on those failures.

**What it does NOT catch**: a JSON config that is well-formed JSON but
omits a required XML attribute (e.g. a `<mass>` object with no `"kg"`
key) reaches the unchanged `Data*::add()` machinery exactly as before,
and the same unchecked `element->attribute(name)->realValue()` call
dereferences a `NULL` `DataParseAttribute*` — a segfault, not a clean
error message.

**Why this wasn't patched at the `DataParseElement::attribute()`
boundary** (the approach floated in the Stage 1 task brief): `attribute()`
is used in two shapes throughout the 80+ `Data*::add()` bodies —
(a) unchecked `attribute(x)->value()` for attributes the schema
guarantees are present, and (b) `if (attribute(x) != NULL) ...` /
`element->set(...)` (which itself null-checks) for attributes that are
**legitimately optional** and fall back to a C++ default when absent
(confirmed in §6: the schema has no default-injection, defaults live in
each `Data*` class's own field initializers). `attribute()` has no way to
distinguish "required, should abort if missing" from "optional, NULL is
the normal not-provided case" — it's the same function serving both call
shapes. Making it abort-on-miss unconditionally (e.g. via a virtual
override in a JSON-specific `DataParseElement` subclass) would fix the
required-attribute case but break every legitimately-optional attribute
across the whole `Data*` hierarchy, which is a strictly worse regression
than the gap it would close. That generic fix needs per-attribute
required/optional metadata — which is exactly what Stage 4's binding
tables are for. Patching this properly is deferred to Stage 4, not
attempted here.

**User-visible statement of the gap (per-brief requirement)**: JSON
configs are **structurally unvalidated** for Stage 1 — the JSON reader
will crash (not cleanly error) on a config that is valid JSON but
violates a schema constraint the XSD would have caught (missing required
attribute, wrong element nesting the schema would reject, etc.). This is
acceptable for Stage 1 because JSON configs are currently only produced by
`XmlToJson` from already-schema-valid XML, so this gap is not reachable
through the Stage 0→1 pipeline as shipped — it only bites hand-written or
hand-edited `.json` configs. (Review addendum 2026-07-07: one indirect
path exists — `XmlToJson` parses with validation disabled (`Val_Never`),
so converting a well-formed but schema-INVALID .xml yields a .json that
crashes at runtime where the .xml itself would have been rejected by
XSD. Convert only known-valid configs until Stage 4 lands.) **The spec's original validation promise
(reject malformed config input cleanly) is only fully restored once ALL
`Data*` families have Stage 4 binding tables with required/optional
attribute metadata.** Explicit sign-off on this gap is needed at the
Stage 1 → Stage 2 boundary before JSON configs are treated as a
first-class, hand-editable format rather than a converter-only
intermediate.
