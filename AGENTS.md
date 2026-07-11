# image-preprocess Guidelines

## Structure

- Implementations: `src/image-preprocess/`
- Manifest: `configs/plugins/`

## Build

From `satellite-workspace`: `./scripts/build-all.sh` or Meson in this directory after SDK wrap is configured.

Contract tests: `scripts/contract-test.sh`

## Coding Style

C++17, `warning_level=2`. Format with repo-root `.clang-format`.

When working inside `satellite-workspace`, see `skills/coding-style-rules/SKILL.md` at the workspace root. Standalone clone: follow the same rules locally.

- **Naming**: PascalCase types; `snake_case` functions and files.
- **API docs**: Doxygen `/** ... */` on public plugin APIs.
- **No silent fallback**; image format and processing assumptions must be explicit.

Details: workspace `skills/coding-style-rules/references/cpp-style-details.md`.
