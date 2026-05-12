## Why

The XSD/XML validation audit (`add-xml-xsd-validation`, run 2026-05-12) discovered that
the YARS XSD generator emits a non-deterministic content model for
`sensor_logging_definition`. xmllint refuses to compile the schema:

```
complex type 'sensor_logging_definition': The content model is not determinist.
```

The generator code at `src/yars/configuration/data/DataLoggingSensor.cpp:58-80` wraps two
`<xs:sequence>` branches inside an `<xs:choice>` — branch A is `<internal>?<external>?`,
branch B is `<external>?<internal>?`. Both branches have all-optional content, so a
document beginning with `<internal/>` can match either branch, violating the W3C Unique
Particle Attribution rule. The audit applied a hand-edit in `xml/yars.xsd` to substitute
`<xs:all>` for the broken construct so corpus validation can run; this proposal removes
the need for that hand-edit by fixing the generator.

The proper construct is `<xs:all>`, which expresses "each child at most once, any
order". The current generator framework has no `XsdAll` primitive, and adding one is a
broader refactor than this fix warrants. Instead, the audit observed that **every config
in the corpus uses the `<external/><internal/>` order** (none uses the reverse), so a
deterministic single-sequence emit is a safe behavioral narrowing.

## What Changes

- In `DataLoggingSensor::createXsd`, replace the `<choice>` of two ambiguous
  `<sequence>` branches with a single `<sequence>` containing optional `<internal/>`
  followed by optional `<external/>`. The schema becomes deterministic without losing
  support for any in-corpus config.
- Regenerate `xml/yars.xsd.regen` from the fixed generator. The post-fix regen output
  matches the hand-edited `xml/yars.xsd` modulo the `<xs:all>` choice — `xml/yars.xsd`
  is then resynced from the new regen, removing the hand-edit annotation.

## Capabilities

### New Capabilities

<!-- None -->

### Modified Capabilities

<!-- None — the generated schema continues to accept every config in the
audit corpus; only the unused alternative order is no longer permitted. -->

## Impact

- `src/yars/configuration/data/DataLoggingSensor.cpp` — `createXsd` simplified, ~15
  lines deleted, ~5 added.
- `xml/yars.xsd` and `xml/yars.xsd.regen` regenerated; `yars.xsd` no longer carries the
  hand-edit comment.
- `make test_xsd_validate` must still produce 27 / 27 PASS.
- Follow-up `add-xsd-all-primitive` (not yet filed) would generalize the XSD framework
  with a real `XsdAll` so the original both-orders intent can be restored.
