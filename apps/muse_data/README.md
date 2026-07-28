# MUSE — muse_data

Import, validate, encode and read point (source) data in the MUSE format:
CSV reading, coordinate/column setup, consistency checks on variables, conversion
to the MUSE format and preliminary statistics.

## Command-line reference

The complete, always up-to-date reference for every option of `muse_data` is
generated from the source code and available at:

- Markdown: ../../docs/md/muse_data.md
- HTML:     ../../docs/html/muse_data.html

or directly from the tool:

```bash
muse_data --help
```

## Runnable examples

End-to-end workflows that use `muse_data` are provided under ../../examples
(see [../../examples/README.md](../../examples/README.md)); every example imports its
data with `muse_data` in its `script/10_test_MUSE_*.sh` pipeline.
