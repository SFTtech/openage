[![openage](/assets/logo/banner.png)](http://openage.sft.mx)
============================================================

**openage**: a volunteer project to create a free engine clone of the *Genie Engine* used by *Age of Empires*, *Age of Empires II (HD)* and *Star Wars: Galactic Battlegrounds*, comparable to projects like [OpenMW](https://openmw.org/), [OpenRA](http://openra.net/),  [OpenSAGE](https://github.com/OpenSAGE/OpenSAGE/), [OpenTTD](https://openttd.org/) and [OpenRCT2](https://openrct2.org/). At the moment we focus our efforts on the integration of *Age of Empires II*, while being primarily aimed at POSIX platforms such as **GNU/Linux**.

openage uses the original game assets (such as sounds and graphics), but (for obvious reasons) doesn't ship them.
To play, you require *an original AoE II: TC or [AoE II: HD](http://store.steampowered.com/app/221380/)* installation
(via [Wine](https://www.winehq.org/) or [Steam-Linux](doc/media_convert.md#how-to-use-the-original-game-assets)).

[![github stars](https://img.shields.io/github/stars/SFTtech/openage.svg)](https://github.com/SFTtech/openage/stargazers)
[![#sfttech on Freenode](https://img.shields.io/badge/chat-on%20freenode-brightgreen)](https://webchat.freenode.net/?channels=sfttech)
[![#sfttech on matrix.org](https://img.shields.io/badge/matrix-%23sfttech-blue.svg)](https://app.element.io/#/room/#sfttech:matrix.org)
[![quality badge](https://img.shields.io/badge/cuteness-overload-orange.svg)](http://www.emergencykitten.com/)



Contact
-------
Contact          | Where?
-----------------|-------
Issue Tracker    | [SFTtech/openage](https://github.com/SFTtech/openage/issues)
Development Blog | [blog.openage.dev](https://blog.openage.dev)
Forum            | [<img src="https://www.redditstatic.com/about/assets/reddit-logo.png" alt="reddit" height="22"/> /r/openage](https://www.reddit.com/r/openage/)
Matrix Chat      | [`#sfttech:matrix.org`](https://app.element.io/#/room/#sfttech:matrix.org)
IRC Chat         | [`irc.freenode.net #sfttech`](https://webchat.freenode.net/?channels=sfttech)
Money Sink       | [![money sink](https://liberapay.com/assets/widgets/donate.svg)](https://liberapay.com/SFTtech)


Technical foundation
--------------------

Technology     | Component
---------------|----------
**C++17**      | Engine core
**Python3**    | Scripting, media conversion, in-game console, code generation
**Qt5**        | Graphical user interface
**Cython**     | Glue code
**CMake**      | Build system
**OpenGL3.3**  | Rendering, shaders
**SDL2**       | Cross-platform Audio/Input/Window handling
**Opus**       | Audio codec
[**nyan**](https://github.com/SFTtech/nyan) | Content Configuration and Modding
**Humans**     | Mixing together all of the above


Goals
-----

* Fully authentic look and feel
    * This can only be approximated, since the behaviour of the original game is mostly undocumented,
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

**Important notice**: Gameplay is currently non-functional as the internal simulation is replaced by a more sophisticated implementation. You also might experience errors when running a build. Gameplay will return in a later update. Detailed explanations can be found in this [blog post](https://blog.openage.dev/new-gamestate-2020.html).

* What features are currently implemented?
    * See [status page](https://github.com/SFTtech/openage/projects).

* What's the plan?
    * See [doc/milestones.md](/doc/milestones.md). We also have [lists of crazy xor good ideas](/doc/ideas) and a [technical overview for requested features](/doc/ideas/fr_technical_overview.md).


Installation Packages
---------------------

**Supported Platforms:** Linux, Windows 10, MacOS X 10.14
 
* For **Linux** check at [repology](https://repology.org/project/openage/versions) if your distribution has any packages available or [here](https://bintray.com/simonsan/openage-packages/openage-linux-releases) for any future updates on *.deb and AppImage packages.
* For **Windows** check our [release page](https://github.com/SFTtech/openage/releases) for the latest installer or [here](https://dl.bintray.com/simonsan/openage-packages/) for nightly builds.

    * **NOTE:** If you have any problems starting conversion or starting *openage* take a look into the [package instructions](doc/build_instructions/packages.md) and our [troubleshooting guide](/doc/troubleshooting.md).

    * **NOTE:** For **MacOSX** we currently don't have any packages.


Quickstart
----------------------------------

  Operating System    | Build status
  :------------------:|:--------------:
  Debian Sid          | [Todo: Kevin #11](https://github.com/SFTtech/kevin/issues/11)
  MacOSX 10.14        | [![Build Status](https://travis-ci.org/SFTtech/openage.svg?branch=master)](https://travis-ci.org/SFTtech/openage)
  Windows 10 - x64    | [![Build status](https://ci.appveyor.com/api/projects/status/66sx35key94u740e?svg=true)](https://ci.appveyor.com/project/simonsan/openage-sl215)


* How do I get this to run on my system?
    * See [doc/building.md](/doc/building.md).

* I compiled everything. Now how do I run it?
    * Execute `bin/run`.
    * [The convert script](/doc/media_convert.md) will transform original assets into openage formats, which are a lot saner and more moddable.
    * Use your brain and react to the things you'll see.

* Waaaaaah! It
    * segfaults
    * prints error messages I don't want to read
    * ate my dog

All of those are features, not bugs.

To turn them off, use `./run --dont-segfault --no-errors --dont-eat-dog`.


If this still does not help, try our [troubleshooting guide](/doc/troubleshooting.md), the [contact section](#contact)
or the [bug tracker](https://github.com/SFTtech/openage/issues).


Contributing
============

You might ask yourself now "Yeah, this sounds cool and all, but how do *I* participate
and ~~get famous~~ contribute useful features?".

Fortunately for you, there is a lot to do and we are very grateful for help.

## Where do I start?

* The engine has several [core parts](https://github.com/SFTtech/openage/projects) that need help.
  You can look at the project related issues and find something for you, for example:
    * **Asset Converter:** Converts whatever properietary format used by a Age of Empires 2 into
    open formats. Written mostly in Python 3. There are a lot of TODOs and beginner issues available
    right now, so it's a good place to get your feet wet.
    * **Game simulation:** Also known as the gameplay implementation. Written in C++, using the
    Entity-Component-System paradigm in addition to an event-driven simulation.
    * **Documentation:** We not only document code, but also anything technical about the Genie engine
    and its games. If you like documenting [file formats](/doc/media)
    or thouroughly investigating [game mechanics](/doc/reverse_engineering),
    then this might be the right place to start.
* **Check the issues** [labelled with good first issues](https://github.com/SFTtech/openage/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22). These are tasks that you can start right away
  and don't require much previous knowledge.
* **Ask us** in the [chat](https://app.element.io/#/room/#sfttech:matrix.org). Someone there could need
  help with something.
* You can also **take the initiative** and fix a bug you found, create an issue for discussion or
  implement a feature that we never though of, but always wanted.


## Ok, I found something. What now?

* **[Tell us](#contact)**, if you haven't already. Chances are that we have additional information
  and directions.
* **[Read the docs](/doc)**. They will answer most "administrative"
  questions like what code style is used and how the engine core parts are connected.
* **Read the code** and get familiar with the engine component you want to work with.
* Do not hesitate to **[ask us for help](#contact)** if you do not understand something.


## How do I contribute my features/changes?

* Read the **[contributing guide](/doc/contributing.md)**.
* You can upload work in progress (WIP) revisions or drafts of your contribution to get feedback or support.
* Tell us (again) when you want us to review your work.

## I want to help, but I'm not a programmer...

Then openage might be a good reason to become one! We have many issues and tasks for beginners. You
just have to ask and we'll find something. Alternatively, lurking is also allowed.

----

Cheers, happy hecking.


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
