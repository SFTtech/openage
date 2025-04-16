[![openage](/assets/logo/banner.svg)](http://openage.dev)
=========================================================

**openage**: a volunteer project to create a free engine clone of the *Genie Engine* used by *Age of Empires*, *Age of Empires II (HD)* and *Star Wars: Galactic Battlegrounds*, comparable to projects like [OpenMW](https://openmw.org/), [OpenRA](http://openra.net/),  [OpenSAGE](https://github.com/OpenSAGE/OpenSAGE/), [OpenTTD](https://openttd.org/) and [OpenRCT2](https://openrct2.org/).

openage uses the original game assets (such as sounds and graphics), but (for obvious reasons) doesn't ship them.
To play, you require *[any of the original games (AoE1, AoE2)](/doc/media_convert.md)* or their *Definitive Edition* releases.

[![github stars](https://img.shields.io/github/stars/SFTtech/openage.svg)](https://github.com/SFTtech/openage/stargazers)
[![#sfttech on matrix.org](/assets/doc/matrixroom.svg)](https://matrix.to/#/#sfttech:matrix.org)
[![GPL licensed](/assets/doc/license.svg)](/legal/GPLv3)


Contact
-------
| Contact          | Where?                                                                                             |
| ---------------- | -------------------------------------------------------------------------------------------------- |
| Issue Tracker    | [GitHub SFTtech/openage]                                                                           |
| Development Blog | [blog.openage.dev]                                                                                 |
| Subreddit        | [![reddit](/assets/doc/reddit.svg) /r/openage](https://www.reddit.com/r/openage/)                  |
| Discussions      | [GitHub Discussions]                                                                               |
| Matrix Chat      | [![matrix](/assets/doc/matrix.svg) `#sfttech:matrix.org`](https://matrix.to/#/#sfttech:matrix.org) |
| Money Sink       | [![money sink](/assets/doc/liberapay.svg)](https://liberapay.com/SFTtech)                          |

[GitHub SFTtech/openage]: https://github.com/SFTtech/openage/issues
[blog.openage.dev]: https://blog.openage.dev
[GitHub Discussions]: https://github.com/SFTtech/openage/discussions

Technical foundation
--------------------

| Technology   | Component                                                     |
| ------------ | ------------------------------------------------------------- |
| **C++20**    | Engine core                                                   |
| **Python3**  | Scripting, media conversion, in-game console, code generation |
| [**Cython**] | Python/C++ Glue code                                          |
| [**Qt6**]    | Graphical user interface                                      |
| [**CMake**]  | Build system                                                  |
| [**OpenGL**] | Rendering, shaders                                            |
| [**Opus**]   | Audio codec                                                   |
| [**nyan**]   | Content Configuration and Modding                             |
| [**Humans**] | Mixing together all of the above                              |

[**Cython**]: https://cython.org/
[**Qt6**]: https://contribute.qt-project.org/
[**CMake**]: https://cmake.org/
[**OpenGL**]: https://www.opengl.org/
[**Opus**]: https://opus-codec.org/
[**nyan**]: https://github.com/SFTtech/nyan
[**Humans**]: https://www.youtube.com/watch?v=fQGbXmkSArs&t=18s

Goals
-----

* Fully authentic look and feel
    * This can only be approximated since the behavior of the original game is mostly undocumented,
    and guessing/experimenting can only get you this close
    * We will not implement useless artificial limitations (max 30 selectable units...)
* An easily-moddable content format: [**nyan** yet another notation](https://github.com/SFTtech/nyan)
* An integrated Python console and API, comparable to [blender](https://www.blender.org/)
* AI scripting in Python, you can use [machine learning](http://scikit-learn.org/stable/)
    * here is some [additional literature](http://www.deeplearningbook.org/)
* Re-creating [free game assets](https://github.com/SFTtech/openage-data)
* Multiplayer (obviously)
* Matchmaking and ranking with a [haskell masterserver](https://github.com/SFTtech/openage-masterserver)
* Optionally, [improvements](/doc/ideas/) over the original game
* Awesome infrastructure such as our own [Kevin CI service](https://github.com/SFTtech/kevin)

But beware, for sanity reasons:

* No network compatibility with the original game.
  You really wanna have the same problems again?
* No binary compatibility with the original game.
  A one-way script to convert maps/savegames/missions to openage is planned though.


Current State of the Project
----------------------------

**Important notice**: At the moment, "gameplay" is basically non-functional.
We're implementing the internal game simulation (how units even do anything) with simplicity and extensibility in mind, so we had to get rid of the temporary (but kind of working) previous version.
With these changes, we can (finally) actually make use of our converted asset packs and our nyan API!
We're working day and night to make gameplay return\*.
If you're interested, we wrote detailed explanations on our blog: [Part 1](https://blog.openage.dev/new-gamestate-2020.html), [Part 2](https://blog.openage.dev/engine-core-modules.html), [Monthly Devlog](https://blog.openage.dev/tag/news.html).

*\* may not actually be every day and night*

|  Operating System   |                                                                                                       Build status                                                                                                        |
| :-----------------: | :-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
|     Debian Sid      | [![Kevin CI status](https://cidata.sft.lol/openage/branches/master/status.svg)](/kevinfile) |
|  Ubuntu 24.04 LTS   |           [![Ubuntu 24.04 build status](https://github.com/SFTTech/openage/actions/workflows/ubuntu-24.04.yml/badge.svg?branch=master)](https://github.com/SFTtech/openage/actions/workflows/ubuntu-24.04.yml)            |
|        macOS        |                              [![macOS build status](https://github.com/SFTtech/openage/workflows/macOS-CI/badge.svg)](https://github.com/SFTtech/openage/actions?query=workflow%3AmacOS-CI)                               |
| Windows Server 2019 | [![Windows Server 2019 build status](https://github.com/SFTtech/openage/actions/workflows/windows-server-2019.yml/badge.svg?branch=master)](https://github.com/SFTtech/openage/actions/workflows/windows-server-2019.yml) |
| Windows Server 2022 | [![Windows Server 2022 build status](https://github.com/SFTtech/openage/actions/workflows/windows-server-2022.yml/badge.svg?branch=master)](https://github.com/SFTtech/openage/actions/workflows/windows-server-2022.yml) |

[Todo: Kevin #11]: https://github.com/SFTtech/kevin/issues/11


Installation Packages
---------------------

There are many missing parts for an actually working game.
So if you "just wanna play", [you'll be disappointed](#current-state-of-the-project), unfortunately.

We strongly recommend building the program from source to get the latest, greatest, and shiniest project state :)


* For **Linux** check at [repology](https://repology.org/project/openage/versions) if your distribution has any packages available. Otherwise, you need to build from source.
  We don't release `*.deb`, `*.rpm`, Flatpak, snap or AppImage packages yet.
* For **Windows** check our [release page](https://github.com/SFTtech/openage/releases) for the latest installer.
  Otherwise, you need to build from the source.

* For **macOS** we currently don't have any packages, you need to build from source.

If you need help, maybe our [troubleshooting guide](/doc/troubleshooting.md) helps you.


Quickstart
----------

* **How do I get this to run on my box?**
    1. [Clone](https://docs.github.com/repositories/creating-and-managing-repositories/cloning-a-repository) the repo.
    2. Install dependencies. See [doc/building.md](/doc/building.md#dependency-installation) to get instructions for your favorite platform.
    3. Build the project:
   ```
   ./configure --download-nyan
   make
   ```

**Alternative approach:**
You can build and run the project using Docker. See [Running with docker](/doc/build_instructions/docker.md) for more details.

* **I compiled everything. Now how do I run it?**
    * Execute `cd bin && ./run main`.
    * [The convert script](/doc/media_convert.md) will transform original assets into openage formats, which are a lot saner and more moddable.
    * Use your brain and react to the things you'll see.

* **Waaaaaah! It...**
    * segfaults
    * prints error messages I don't want to read
    * ate my dog

All of those are features, not bugs.

To turn them off, use `./bin/run --dont-segfault --no-errors --dont-eat-dog`.


If this still does not help, try our [troubleshooting guide](/doc/troubleshooting.md), the [contact section](#contact)
or the [bug tracker](https://github.com/SFTtech/openage/issues).

Contributing
============

You might ask yourself now "Sounds cool, but how do I participate
and ~~get famous~~ contribute useful features?".

Fortunately for you, there is a lot to do and we are very grateful for your help.

## Where do I start?

* **Check the issues** [labelled with `good first issue`](https://github.com/SFTtech/openage/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22). These are tasks that you can start right away and don't require much previous knowledge.
* **Ask us** in the [chat](https://matrix.to/#/#sfttech:matrix.org). Someone there could need
  help with something.
* You can also **take the initiative** and fix a bug you found, create an issue for discussion or
  implement a feature that we never thought of, but always wanted.


## Ok, I found something. What now?

* **[Tell us](#contact)**, if you haven't already. Chances are that we have additional information
  and directions.
* **[Read the docs](/doc)**. They will answer most "administrative"
  questions like what code style is used and how the engine core parts are connected.
* **Read the code** and get familiar with the engine component you want to work with.
* Do not hesitate to **[ask us for help](#contact)** if you do not understand something.


## How do I contribute my features/changes?

* Read the **[contributing guide](/doc/contributing.md)**.
* You can upload work-in-progress (WIP) versions or drafts of your contribution to get feedback or support.
* Tell us (again) when you want us to review your work.

## I want to help, but I'm not a programmer...

Then openage might be a good reason to become one! We have many issues and tasks for beginners. You
just have to ask and we'll find something. Alternatively, lurking is also allowed.

----

Cheers, happy hecking!


Development Process
-------------------

What does openage development look like in practice?

* extensive [synchronization](#contact)!
* [doc/development.md](/doc/development.md).

How can I help?

* [doc/contributing.md](/doc/contributing.md).

All documentation is also in this repo:

* Code documentation is embedded in the sources for Doxygen (see [doc readme](/doc/README.md)).
* Have a look at the [doc directory](/doc/). This folder tends to get outdated when code changes.


License
-------

**GNU GPLv3** or later; see [copying.md](copying.md) and [legal/GPLv3](/legal/GPLv3).

I know that probably nobody is ever gonna look at the `copying.md` file,
but if you want to contribute code to openage, please take the time to
skim through it and add yourself to the authors list.
