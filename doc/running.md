# How to run openage?

This document explains the different run modes in openage.

1. [Quickstart](#quickstart)
2. [Modes](#modes)
   1. [`game`](#game)
   2. [`main`](#main)
   3. [`test`](#test)
   4. [`convert`](#convert)
   5. [`convert-file`](#convert-file)
   6. [`convert-export-api`](#convert-export-api)
   7. [`codegen`](#codegen)


## Quickstart

After building the project with the commands

```
./configure
make
```

you can execute

```
bin/run
```

from the same subfolder. This automatically selects the [`game` mode](#game) as default
to start a new game instance and also creates all necessary configs.

If prompts appear, follow the instructions and choose what you think is best. It's
almost idiot-proof!


## Modes

Modes can be selected manually by appending the `bin/run` prompt with the mode name.

### `game`

```
bin/run game
```

Start the engine and immediately create a new game instance. This run mode is supposed
to jump straight into a game (or replay recording).

If no converted modpacks can be found, this mode will start with a prompt asking if
the user wants to convert any before initializing the game.

It's the default run mode.


### `main`

```
bin/run main
```

This run mode is supposed to start a main menu or launcher which allows configuring a
game. Neither of these are implemented at the moment, so `main` just does the same
thing as `game`.


### `test`

```
bin/run test
```

Used for running [tests and engine demos](code/testing.md). These show off selected
subsystems of the engine.


### `convert`

```
bin/run convert
```

Runs the [asset conversion](media_convert.md) subsystem which creates openage modpacks
from original game installations.


### `convert-file`

```
bin/run convert-file
```

Allows converting single media files using the original game formats to open formats
(PNG for graphics, OPUS for sounds).


### `convert-export-api`

```
bin/run convert-export-api
```

Exports the [openage modding API](nyan/README.md) nyan objects to a modpack that can
be loaded by the engine.

This is supposed to be temporary solution until the modding API is finalized. Currently,
the modding API nyan objects are hardcoded into the converter. In the future, the
modding API modpack should be part of the engine config which is loaded by both the engine
the converter subsystem as a single source of truth.


### `codegen`

```
bin/run codegen
```

Runs the code generation logic for the build process. In particular, this generates code
for [mako](https://www.makotemplates.org/) templates and creates the test lists for the
[testing](code/testing.md) subsystem.
