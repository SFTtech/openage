# AGENTS.md

openage is a free-software RTS engine recreating Age of Empires II: `libopenage/` is the C++20 engine, `openage/` the Python package (converter, codegen, nyan integration), glued together by Cython

## Reference docs

Read the docs of the subsystem you touch; link to them instead of duplicating their content

- [doc/project_structure.md](doc/project_structure.md) — folder map
- [doc/building.md](doc/building.md) — dependencies (flags C/R/A/S/O) and build; per-platform setup incl. Nix in [doc/build_instructions/](doc/build_instructions/)
- [doc/buildsystem.md](doc/buildsystem.md) — codegen → pxdgen → cythonize pipeline
- [doc/code/architecture.md](doc/code/architecture.md) — subsystems and data flow, details in its sibling dirs (`event/`, `game_simulation/`, `input/`, `renderer/`, `pathfinding/`, `converter/`)
- [doc/code/](doc/code/) — [curves](doc/code/curves.md), [exceptions](doc/code/exceptions.md), [logger](doc/code/logger.md), [pyinterface](doc/code/pyinterface.md), [testing](doc/code/testing.md)
- [doc/code_style/](doc/code_style/) — C++ by example in [mom.h](doc/code_style/mom.h) and [mom.cpp](doc/code_style/mom.cpp), tabs indent and spaces align; Python is a PEP8 subset
- [doc/nyan/](doc/nyan/), [doc/convert/](doc/convert/), [doc/media_convert.md](doc/media_convert.md) — game data and converter
- [doc/development.md](doc/development.md) — code philosophy; [doc/contributing.md](doc/contributing.md) — PR workflow

## Build and verify

`./configure` creates the build dir in `.bin/`, symlinked as `bin/`; the root `Makefile` forwards there

```sh
./configure # e.g. --compiler=clang --mode=release --ccache --download-nyan
make -j$(nproc) # build
make run # run the game
make tests # all C++/Python tests and doctests, as CI runs them
make checkmerge # compliance checks, as CI runs them
make doc # doxygen → bin/doc/html/index.html
```

- Single test: `bin/run test NAME`, e.g. `openage::curve::tests::container`; `-l` lists all, `-d NAME ARGS` runs a demo, `-b NAME` a benchmark, `-a --have-assets` includes tests needing game assets
- Checks on changed files only: `make checkuncommited` (sic; vs `HEAD`, new files count once `git add`ed) or `make checkchanged` (vs `origin/master`)
- `checkfast`: authors, cppstyle, cython, headerguards, legal, filemodes, textfiles; `checkmerge` adds pystyle and pylint ([etc/pylintrc](etc/pylintrc)); `checkall` adds clang-tidy
- Memory or threading changes: also run the tests in a build configured with `--sanitize=yes` (ASan+UBSan) or `--sanitize=thread`
- CI also builds on macOS and Windows ([.github/workflows/](.github/workflows/)): keep code portable

## Architecture essentials

- No ticks: the simulation is event-driven and all gamestate lives in time-indexed curves ([curves.md](doc/code/curves.md)); schedule events (`event/`) instead of adding per-frame updates
- Presenter, simulation and time run in separate threads and communicate only via defined interfaces; renderer and input are optional, the simulation must run without them
- Subsystems talk only through their interfaces, never through each other's internals, e.g. gamestate reaches the renderer only via render entities, never OpenGL
- Simulation time and world coordinates are fixed-point (`time::time_t`, `coord::phys_t`); don't model them as `float`/`double`
- Game data is [nyan](https://github.com/SFTtech/nyan), converted from the original game files by `bin/run convert` into `assets/converted/` (git-ignored, never built in)
- The nyan API is mirrored in [doc/nyan/api_reference/](doc/nyan/api_reference/), `openage/convert/service/read/nyan_api_loader.py` and `libopenage/gamestate/api/`: change them together, then reconvert
- Rendering: OpenGL via libepoxy (Vulkan in progress); GUI: Qt6 Quick

## Pitfalls

Non-obvious traps; add new ones here, verified and in one line

- In-source builds are forbidden and poison the tree; `make cleaninsourcebuild` removes the traces
- Run Python only via `bin/run ...` or `make run`, never `python3 -m openage` in the source tree: compiled Cython modules and `openage/config.py` live in `bin/`
- Never edit files in `bin/`, they are generated: `config.*` from `*.in`, `*.gen.{h,cpp}` from `openage/codegen/` and `*.template`, `libopenage` `.pxd` files from `pxd:` annotations in C++ headers; codegen trouble: `make cleancodegen`
- nyan is a separate project: build it and point to it with `-Dnyan_DIR=`, or `./configure --download-nyan`
- C++20, but the minimum compilers are gcc/clang 10 and MSVC 16.8 (`buildsystem/CheckCompilerFeatures.cmake`): check support before using newer library features like `std::format`

## Working

- Read the surrounding code first and follow its patterns instead of inventing parallel ones
- Do things properly, not quickly ([development.md](doc/development.md#code-philosophy)): prefer the principled fix; if it needs broad changes, propose the approach and confirm the scope first
- Broken or missing upstream (nyan, Cython, compilers, libraries): fix or report it there; an unavoidable hack gets a `TODO:` explaining it
- Internal interfaces may change: update all callers instead of adding compatibility shims; ask before changing file formats (nyan API, modpacks, cfg files)
- Ask when requirements are unclear instead of guessing
- Keep diffs focused: no drive-by refactors, reformatting, renames or "improvements" outside the task
- Fail hard: throw/raise on unexpected state instead of skipping, logging and continuing, or returning a default
- Trust the code over comments and docs; fix stale comments in code you touch
- Done means it builds, the relevant tests pass and `make checkmerge` is clean; state what you could not verify, e.g. without game assets or GPU
- Scratch files go to `/tmp`, never into the tree

## Code

- Use the first that applies: YAGNI, stdlib, existing utilities (`libopenage/util/`, `openage/util/`), an existing dependency, minimal new code
- One source of truth: reuse, adapt or extend existing code instead of duplicating it, e.g. share converter logic between game editions instead of copying it per edition
- New dependencies need approval and must be added to [doc/building.md](doc/building.md), [doc/build_instructions/](doc/build_instructions/), CMake, `nix/` and `packaging/`
- Don't hardcode what belongs in a config file, cvar or CLI option
- No dead code, commented-out code, debug output or unexplained `TODO`s
- Format only the lines you change: `clang-format` for C++, never whole files (many aren't clang-format clean); use `ruff format` for Python
- Don't pad consecutive lines into columns and never realign existing ones; aligning continuation lines is fine ([tabs_n_spaces.md](doc/code_style/tabs_n_spaces.md))
- Separate logical blocks with empty lines
- Comment why, never what
- Never mention development phases, "scaffolding" or AI assistance in code, docs or commits

## C++

- Style by example ([mom.h](doc/code_style/mom.h), [mom.cpp](doc/code_style/mom.cpp)): `this->` for members, `{}` initialization, braces on every block, Doxygen API docs in the header, forward declarations in headers, sorted include groups
- Every `.h` has a `.cpp` that includes it first, even if otherwise empty; header guard `#pragma once`
- Register new `.cpp` files and directories in the local `CMakeLists.txt` (`add_sources(libopenage ...)`, `add_subdirectory`)
- Integer types per [inttypes.md](doc/code_style/inttypes.md)
- RAII and smart pointers, no owning raw `new`/`delete`
- Log with `log::log(INFO << ...)`, throw `Error{MSG(err) << ...}` ([exceptions.md](doc/code/exceptions.md)); no `std::cout`/`printf`, no bare `std::` exceptions

## Python and Cython

- Target `PYTHON_MIN_VERSION` from [CMakeLists.txt](CMakeLists.txt)
- Register new files in the local `CMakeLists.txt` (`add_py_modules`, `add_cython_modules`, `add_pxds`); cmake fails on unlisted `.py` files
- Declare C++ functions `except +` in their `pxd:` annotation unless they are `noexcept`, otherwise a C++ exception crashes the interpreter ([pyinterface.md](doc/code/pyinterface.md))
- Diagnostics via `openage.log` (`dbg`, `info`, `warn`, ...), not `print`
- Type-annotate new functions like the surrounding code
- Keep pylint clean; a `# pylint: disable=...` needs a reason and the narrowest scope

## Tests

Write tests where they catch real bugs; the right kind depends on the subject:

- Data structures, algorithms, math, curves, events, pathfinding: unit tests of invariants and edge cases (empty, boundaries, overflow, equal timestamps, queries exactly at, before and after keyframes)
- Parsers and converter logic: small hand-made inputs; gate tests needing game assets on `has_assets` in the test list
- Small pure Python functions: doctests, which double as documentation
- Rendering, GUI, input wiring and other interactive code: usually no automated test; verify with a demo or `make run` and say so
- Performance claims: measure with a benchmark (`bin/run test -b NAME`)
- Bug fixes: a regression test that fails without the fix, where the subject is testable as above
- Test observable behavior and contracts, not implementation details; no tests that restate the code or depend on timing, network or user interaction
- Never weaken, skip or delete a test to make it pass; fix the code or ask

Mechanics ([testing.md](doc/code/testing.md)):

- Register every test, demo and benchmark in `openage/testing/testlist.py`
- C++: `void` function in namespace `openage`, not declared in a header, failing via `TESTFAIL`, `TESTEQUALS`, `TESTTHROWS`, ... from `libopenage/testing/testing.h`
- Python: argument-less function raising `openage.testing.testing.TestError`, e.g. via `assert_value` or `assert_raises`

## Writing docs

- Update `doc/` when behavior, interfaces or file formats change
- Write for the reader, not as a record of decisions or attempts
- Markdown: one sentence per line

## Commits

- Message `subsystem: summary`, e.g. `convert: ...`, `renderer: ...`; the body explains what and why, concisely
- One logical change per commit; squash fixups before review ([contributing.md](doc/contributing.md#rebasing))
- New source files start with `// Copyright <year> the openage authors. See copying.md for legal info.` (`#` in Python and CMake) and have mode 0644; don't bump years in existing headers
- Commit authors must be listed in [copying.md](copying.md) (authors check)
- Never commit generated files, build output, converted assets or scratch files
- Never push, force-push, open or comment on PRs without explicit consent
