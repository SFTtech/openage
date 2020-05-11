[![openage](/assets/logo/banner.png)](http://openage.sft.mx)
============================================================

**openage**: a volunteer project to create a free engine clone of the *Genie Engine* used by *Age of Empires*, *Age of Empires II (HD)* and *Star Wars: Galactic Battlegrounds*, comparable to projects like [OpenMW](https://openmw.org/), [OpenRA](http://openra.net/),  [OpenSAGE](https://github.com/OpenSAGE/OpenSAGE/), [OpenTTD](https://openttd.org/) and [OpenRCT2](https://openrct2.org/). At the moment we focus our efforts on the integration of *Age of Empires II*, while being primarily aimed at POSIX platforms such as **GNU/Linux**.

openage uses the original game assets (such as sounds and graphics), but (for obvious reasons) doesn't ship them.
To play, you require *an original AoE II: TC or [AoE II: HD](http://store.steampowered.com/app/221380/)* installation
(via [Wine](https://www.winehq.org/) or [Steam-Linux](doc/media_convert.md#how-to-use-the-original-game-assets)).

[![github stars](https://img.shields.io/github/stars/SFTtech/openage.svg)](https://github.com/SFTtech/openage/stargazers)
[![#sfttech on Freenode](https://img.shields.io/badge/chat-on%20freenode-brightgreen)](https://webchat.freenode.net/?channels=sfttech)
[![#sfttech on matrix.org](https://img.shields.io/badge/matrix-%23sfttech-blue.svg)](https://riot.im/app/#/room/#sfttech:matrix.org)
[![quality badge](https://img.shields.io/badge/cuteness-overload-orange.svg)](http://www.emergencykitten.com/)



Contact
-------
Contact  | Where?
---------|-------
Issue Tracker | [SFTtech/openage](https://github.com/SFTtech/openage/issues)
Development Blog | [blog.openage.sft.mx](https://blog.openage.sft.mx)
Forum | [<img src="https://www.redditstatic.com/about/assets/reddit-logo.png" alt="reddit" height="22"/> /r/openage](https://www.reddit.com/r/openage/)
Matrix Chat | [`#sfttech:matrix.org`](https://riot.im/app/#/room/#sfttech:matrix.org)
IRC Chat | [`irc.freenode.net #sfttech`](https://webchat.freenode.net/?channels=sfttech)
Money Sink | [![money sink](https://liberapay.com/assets/widgets/donate.svg)](https://liberapay.com/SFTtech)


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
* Multiplayer (obviously)
* Matchmaking and ranking with a [haskell masterserver](https://github.com/SFTtech/openage-masterserver)
* Optionally, [improvements](/doc/ideas/) over the original game
* AI scripting in Python, you can use [machine learning](http://scikit-learn.org/stable/)
  * here is some [additional literature](http://www.deeplearningbook.org/)
* Re-creating [free game assets](https://github.com/SFTtech/openage-data)
* An easily-moddable content format: [**nyan** yet another notation](https://github.com/SFTtech/nyan)
* An integrated Python console and API, comparable to [blender](https://www.blender.org/)
* Awesome infrastructure such as our own [Kevin CI service](https://github.com/SFTtech/kevin)

But beware, for sanity reasons:

* No network compatibility with the original game.
  You really wanna have the same problems again?
* No binary compatibility with the original game.
  A one-way script to convert maps/savegames/missions to openage is planned though.


Current State of the Project
----------------------------

 - What features are currently implemented?
   - See [doc/status.md](/doc/status.md).

 - What's the plan?
   - See [doc/milestones.md](/doc/milestones.md). We also have [lists of crazy xor good ideas](/doc/ideas) and a [technical overview for requested features](/doc/ideas/fr_technical_overview.md). 


Installation Packages
---------------------

**Supported Platforms:** Linux, Windows 10, MacOS X 10.14
 
  - For **Linux** check at [repology](https://repology.org/project/openage/versions) if your distribution has any packages available or [here](https://bintray.com/simonsan/openage-packages/openage-linux-releases) for any future updates on *.deb and AppImage packages.
  - For **Windows** check our [release page](https://github.com/SFTtech/openage/releases) for the latest installer or [here](https://dl.bintray.com/simonsan/openage-packages/) for nightly builds.

    __NOTE:__ If you have any problems starting conversion or starting *openage* take a look into the [package instructions](doc/build_instructions/packages.md) and our [troubleshooting guide](/doc/troubleshooting.md).

    __NOTE:__ For **MacOSX** we currently don't have any packages.


Dependencies, Building and Running
----------------------------------

  Operating System    | Build status
  :------------------:|:--------------:
  Debian Sid          | [Todo: Kevin #11](https://github.com/SFTtech/kevin/issues/11)
  MacOS 10.14         | [![MacOSX builds](https://github.com/SFTtech/openage/workflows/MacOSX%20builds/badge.svg)](https://github.com/SFTtech/openage/actions?query=workflow%3A%22MacOS+10.15%22)
  Windows 10          | [![Windows 10](https://github.com/SFTtech/openage/workflows/Windows%2010/badge.svg?branch=master)](https://github.com/SFTtech/openage/actions?query=workflow%3A%22Windows+10%22)


 - How do I get this to run on my box?
   - See [doc/building.md](/doc/building.md).

 - I compiled everything. Now how do I run it?
   - Execute `./run`.
   * [The convert script](/doc/media_convert.md) will transform original assets into openage formats, which are a lot saner and more moddable.
   - Use your brain and react to the things you'll see.

 - Waaaaaah! It
   - segfaults
   - prints error messages I don't want to read
   - ate my dog

All of those are features, not bugs.

To turn them off, use `./run --dont-segfault --no-errors --dont-eat-dog`.


If this still does not help, try our [troubleshooting guide](/doc/troubleshooting.md), the [contact section](#contact)
or the [bug tracker](https://github.com/SFTtech/openage/issues).


Contributing
============

* Being typical computer science students, we hate people.
* Please don't contact us.
* Nobody likes Age of Empires anyway.
* None of you are interested in making openage more awesome anyway.
* We don't want a community.
* Don't even think about trying to help.

Guidelines:

* No **bug reports** or **feature requests**, the game is perfect as is.
* Don't try to **fix any bugs**, see above.
* Don't implement any features, your code is crap.
* Don't even think about sending a **pull request**.
* Please ignore the [easy tasks](https://github.com/SFTtech/openage/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) that [could just be done](https://github.com/SFTtech/openage/issues?q=is:issue+is:open+label:%22just+do+it%22).
* Absolutely never ever participate in this [boring community](https://www.reddit.com/r/openage/).
* Don't note the irony, you idiot.

To prevent accidental violation of one of those guidelines, you should *never*

* [learn git](https://git-scm.com/book/)
* [fork the repo](https://help.github.com/articles/fork-a-repo)
* [learn python](https://docs.python.org/3/tutorial/appetite.html)
* [learn c++](http://www.cplusplus.com/doc/tutorial/)
* read the code and documentation
* [contribute](/doc/contributing.md) anything to the code
* [contact us](#contact)

Cheers, happy hecking.


Development Process
-------------------

What does openage development look like in practice?
 - extensive [synchronization](#contact)!
 - [doc/development.md](/doc/development.md).

Can I help?
 - [doc/contributing.md](/doc/contributing.md).

All documentation is also in this repo:

- Code documentation is embedded in the sources for Doxygen (see [doc readme](/doc/README.md)).
- Have a look at the [doc directory](/doc/). This folder tends to get outdated when code changes.


License
-------

**GNU GPLv3** or later; see [copying.md](copying.md) and [legal/GPLv3](/legal/GPLv3).

I know that probably nobody is ever gonna look at the `copying.md` file,
but if you want to contribute code to openage, please take the time to
skim through it and add yourself to the authors list.
