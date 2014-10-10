openage
=======

**openage**: a volunteer project to create a free engine clone of *Age of Empires II*,
primarily aimed at POSIX platforms such as **GNU/Linux**,
comparable in its goals to projects like [OpenMW](https://openmw.org/), [OpenRA](http://openra.net/) and [OpenTTD](http://openttd.org/).

openage uses the original game data files (such as sounds and graphics), but (for obvious reasons) doesn't ship them.
To play, you require *an original AoE2: AoC installation* (wine is your friend; in the near future, setup discs will be supported).

[![build status](https://travis-ci.org/SFTtech/openage.png?branch=master)](https://travis-ci.org/SFTtech/openage)

The foundation of **openage**:

Technology     | Component
---------------|----------
**C++11**      | Engine core
**Python**     | Scripting, media conversion, in-game console, code generation
**CMake**      | Build system
**OpenGL2.1**  | Rendering, shaders
**SDL2**       | Cross-platform Audio/Input/Window handling
**Opus**       | Audio codec
**Humans**     | Mixing together all of the above

Our goals *include*:

* A fully authentic gameplay feeling
  * This can only be approximated, since the behaviour of the original game is mostly undocumented,
    and guessing/experimenting can only get you this close
  * We will not implement useless artificial limitations (max 30 selectable units...))
* Multiplayer (obviously)
* [Improvements](doc/ideas/improvements.md) over the original game
* AI
* Re-creating free game assets
* An easily-moddable content format
* A powerful integrated Python console and interface, comparable to [blender](http://blender.org/)

Our goals specifically *exclude*:

* Network compatibility with the original game.
  You really wanna have the same problems again?
* Binary compatibility with the original game.
  A one-way script to convert maps/savegames/missions to openage is planned though.


Current State of the Project
----------------------------

 - What features are currently implemented?

See [status.md](status.md).

 - What's the plan?

See [milestones.md](milestones.md). We also have a [list of crazy xor good ideas](doc/ideas).

Dependencies, Building and Running
----------------------------------

 - How do I get this to run on my box?

See [building.md](building.md).

 - I compiled everything. Now how do I run it?

You first need to use [the convert script](doc/media_convert.md) (will be automated in the near future!) to convert the original game assets to the (a lot saner and more moddable) openage format. Then, you can simply run the game using `./openage --data=datafolder`.

 - Waaaaaah! It
  - segfaults
  - prints error messages I don't want to read
  - ate my dog

All of those are features, not bugs.
To turn them off, use `./openage --dont-segfault --no-errors --dont-eat-dog`.

If this still does not help, try the [contact section](#contact)
or the [bug tracker](https://github.com/SFTtech/openage/issues).

Development Process
-------------------

* How does openage development look in practice?
  * Awesome.

* Can I help?
  * Yes, please!

See [development.md](development.md).


Project documentation is accompanying the source code in the `doc/` folder:

- Have a look at the [doc directory](doc/).
- We use Doxygen, as described in the [doc readme](doc/README.md)
- Have a look at the source.


Windows Version
---------------

None of uses Windows, so a port has no priority.

However, we're using cross-platform libraries wherever possible, so a port should be pretty easy to accomplish. We'll eventually look into poriting using `mingw32`/`mingw64` or maybe `cygwin`.

If you want to beat us to it, go for it!
We'd prefer as few preprocessor switches as possible, preferrably they should all be placed in a few 'cross-platform abstraction files' (just talk to us for details...).

Contributing
============

* Being typical computer science students, we hate people.
* Please don't contact us.
* Nobody likes Age of Empires anyway.
* None of you is interested in making openage more awesome anyway.
* We don't want a community.
* Don't even think about trying to help.

Guidelines:

* No **bugreports** or **feature requests**, the game is perfect as is.
* Don't try to **fix any bugs**, see above.
* Don't implement any features, your code is crap.
* Don't even think about sending a **pull request**
* Don't note the irony, you idiot
* We even have a [list of tasks](tasks.md) that definitely don't need your work.

To prevent accidential violation of one of those guidelines, you should *never*

* [learn git](http://git-scm.com/book/en/Git-Basics)
* [fork the repo](https://help.github.com/articles/fork-a-repo)
* [learn python](http://docs.python.org/3/tutorial/appetite.html)
* [learn c++11](http://www.cplusplus.com/doc/tutorial/)
* read the code and documentation
* contribute anything to the code
* [contact us](#contact)

cheers, happy hecking.

Contact
-------

Most of us hang around on our **IRC** channel (`#sfttech` on `freenode.net`).
Do not hesitate to ping us, we probably won't notice you otherwise.

License
-------
**GNU GPLv3** or later; see [COPYING](COPYING) and [LICENSE](LICENSE).
