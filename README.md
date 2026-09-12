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


## 🌐 Web Resources & Interactive Index
- [CATEGORY SIMULATION](https://eduquests.pages.dev/category-simulation.html)
- [OVERPROTECTIVE BOYFRIEND](https://thebrainquestpark-ja.pages.dev/overprotective-boyfriend.html)
- [EPIC STUNTS PVP 3D](https://learnquest-ru.pages.dev/epic-stunts-pvp-3d.html)
- [SLIME RUSH](https://thebrainquests-hi.pages.dev/slime-rush.html)
- [CATEGORY MATCH 3](https://thelearnplays-pt.pages.dev/category-match-3.html)
- [SURVEV](https://eduquestsjp.pages.dev/survev.html)
- [DRAW TO FLY](https://brainquestses.pages.dev/draw-to-fly.html)
- [TWO CARTS DOWNHILL](https://eduquestses.pages.dev/two-carts-downhill.html)
- [BLOON POP](https://theeduplays9.pages.dev/bloon-pop.html)
- [CATEGORY LOGIC](https://mindconvert.onrender.com/category-logic.html)
- [CATEGORY IDLE445](https://themindinstitutes.pages.dev/category-idle445.html)
- [ITALIAN BRAINROT CHALLENGE](https://jangkhangkr.pages.dev/italian-brainrot-challenge.html)
- [INTERSTELLAR](https://thebrainquests9.pages.dev/interstellar.html)
- [BOOM STICK BAZOOKA](https://eduquestses.pages.dev/boom-stick-bazooka.html)
- [DIAMOND MOSAIC](https://eduquestses.pages.dev/diamond-mosaic.html)
- [INDEX22](https://brainquesteses.pages.dev/index22.html)
- [SAND BLAST](https://thestudyarcades-vi.pages.dev/sand-blast.html)
- [CATEGORY CASUAL969](https://thebrainquests9.pages.dev/category-casual969.html)
- [CATEGORY IO](https://theeduplays-es.pages.dev/category-io.html)
- [KITTEN NEVER DIES](https://brainquestspt.pages.dev/kitten-never-dies.html)
- [CATEGORY SIMULATION 5](https://brainquests.pages.dev/category-simulation-5.html)
- [MAZE ESCAPE CHALLENGE](https://brainquests.pages.dev/maze-escape-challenge.html)
- [PLANETARIUM 2](https://theeduplays-es.pages.dev/planetarium-2.html)
- [CATEGORY SNAKE GAMES](https://jangkhangplay.pages.dev/category-snake-games.html)
- [CATEGORY SURVIVAL366](https://brainquests.pages.dev/category-survival366.html)
- [CUTE CATS ADVENTURES](https://brainquestspt.pages.dev/cute-cats-adventures.html)
- [VISUAL MEMORY DRAG DROP](https://brainquestses.pages.dev/visual-memory-drag-drop.html)
- [HOUSE ROBBER](https://theskillquests9.pages.dev/house-robber.html)
- [TUNNEL ROAD](https://jangkhangkr.pages.dev/tunnel-road.html)
- [INDEX12](https://brainquests.pages.dev/index12.html)
- [GRANNY RETURNS 3D EVIL DESTINY](https://theeduquests9.pages.dev/granny-returns-3d-evil-destiny.html)
- [WONDERS OF EGYPT MATCH](https://brainquests.pages.dev/wonders-of-egypt-match.html)
- [STICKMAN LEAVE PRISON](https://theeduplays-es.pages.dev/stickman-leave-prison.html)
- [CATEGORY AGILITY](https://theeduplays9.pages.dev/category-agility.html)
- [FORCE MASTER 3D](https://brainquestskr.pages.dev/force-master-3d.html)
- [CATEGORY SKILL256](https://mindconvert.netlify.app/category-skill256.html)
- [MONEY PING PONG](https://chuyentestss.pages.dev/money-ping-pong.html)
- [DESIGN WITH ME SUPERHERO TUTU OUTFITS](https://brainquestspt.pages.dev/design-with-me-superhero-tutu-outfits.html)
- [AUTUMN GLAM GALA](https://themindinstitutes.pages.dev/autumn-glam-gala.html)
- [CATEGORY CAR](https://brainquestsfr.pages.dev/category-car.html)
- [CATEGORY 2D1 070](https://thelearningarcades.pages.dev/category-2d1-070.html)
- [MY LITTLE CAR WASH](https://mindconvert.netlify.app/my-little-car-wash.html)
- [GOING BALLS ADVENTURE 2](https://thestudyarcades-vi.pages.dev/going-balls-adventure-2.html)
- [CHRISTMAS FIND THE DIFFERENCES](https://brainquestses.pages.dev/christmas-find-the-differences.html)
- [ASMR GIRL LIVESTREAM MUKBANG](https://brainquestspt.pages.dev/asmr-girl-livestream-mukbang.html)
- [BASKET SPORT STARS](https://theknowledgequests9.pages.dev/basket-sport-stars.html)
- [CATEGORY CASUAL 3](https://thelearningarcades-en.pages.dev/category-casual-3.html)
- [IDLE FACTORY EMPIRE](https://brainquestskr.pages.dev/idle-factory-empire.html)
- [12 IN 1 SOLITAIRE](https://theeduplays-es.pages.dev/12-in-1-solitaire.html)
- [GRAND THEFT NY](https://theeduplays-es.pages.dev/grand-theft-ny.html)
- [CATEGORY FASHION](https://theplayandlearns9.pages.dev/category-fashion.html)
- [WARCALL IO](https://brainquestspt.pages.dev/warcall-io.html)
- [CUTE COLORING GAMES](https://theeduquests9.pages.dev/cute-coloring-games.html)
- [INDEX12](https://mindconvertes.pages.dev/index12.html)
- [SMALL WARDROBE](https://brainquestsjp.pages.dev/small-wardrobe.html)
- [CATEGORY UNBLOCKED GAMES](https://brainquests.pages.dev/category-unblocked-games.html)
- [CATEGORY TRIVIA COLLECTION](https://brainquesteses.pages.dev/category-trivia-collection.html)
- [ONLINE PORTAL](https://themindconvert.web.app/)
- [CATEGORY COLOR197](https://thelearningarcades9.pages.dev/category-color197.html)
- [GRILL PARTY](https://theeduplays-es.pages.dev/grill-party.html)
- [FAMILY SQUID CHALLENGE](https://thestudyquests9.pages.dev/family-squid-challenge.html)
- [GROCERY SHOP SUPERMARKET GAME](https://jangkhangplay.pages.dev/grocery-shop-supermarket-game.html)
- [CUBE DROP PUZZLE](https://themindinstitutes.pages.dev/cube-drop-puzzle.html)
- [PHYSICS BOX 2](https://thelearningarcades.pages.dev/physics-box-2.html)
- [SPACEIO](https://eduquestkr.pages.dev/spaceio.html)
- [IDLE BATHROOM EMPIRE TYCOON](https://themindquests-zh.pages.dev/idle-bathroom-empire-tycoon.html)
- [COLOR SAND PUZZLE](https://jangkhangkr.pages.dev/color-sand-puzzle.html)
- [ZOMBIE DEFENSE WAR](https://brainquesteses.pages.dev/zombie-defense-war.html)
- [COOKING EMPIRE](https://brainquests.pages.dev/cooking-empire.html)
- [FLOW BLOCK](https://mindconvertpt.pages.dev/flow-block.html)
- [SCREW MASTERS 3D PUZZLE](https://mindconvertpt.pages.dev/screw-masters-3d-puzzle.html)
- [CATEGORY CASUAL 7](https://themindquests9.pages.dev/category-casual-7.html)
- [CITY DRIFT RACING](https://thelearnquests9.pages.dev/city-drift-racing.html)
- [CATEGORY TANK58](https://thebrainquests-hi.pages.dev/category-tank58.html)
- [SPRUNKI 3D ESCAPE](https://themindquests9.pages.dev/sprunki-3d-escape.html)
- [CATEGORY GUN238](https://jangkhangplay.pages.dev/category-gun238.html)
- [CROSS THE ROAD](https://theskillquests9.pages.dev/cross-the-road.html)
- [BLOCK MATCH 8X8](https://thelearnplays9.pages.dev/block-match-8x8.html)
- [CATEGORY SANDBOX](https://ieduquests.web.app/category-sandbox.html)
- [CRYSTAL CONNECT](https://eduquestses.pages.dev/crystal-connect.html)
- [SKILLFUL FINGER](https://eduquestses.pages.dev/skillful-finger.html)
- [WILD WEST MATCH 2 THE GOLD RUSH](https://eduquestses.pages.dev/wild-west-match-2-the-gold-rush.html)
- [FISH LAND FISH WORLD](https://brainquesteses.pages.dev/fish-land-fish-world.html)
- [CATEGORY LOVE12](https://thebrainquests9.pages.dev/category-love12.html)
- [DESTINATION BRAIN TEST](https://brainquestspt.pages.dev/destination-brain-test.html)
- [INDEX14](https://thelearnplays-pt.pages.dev/index14.html)
- [SHEEP SHEEP DUCK](https://theskillquests9.pages.dev/sheep-sheep-duck.html)
- [ZUMBA QUEST](https://themindquests9.pages.dev/zumba-quest.html)
- [FARM MATCH SEASONS 2](https://eduquestspt.pages.dev/farm-match-seasons-2.html)
- [CATEGORY DESTROY](https://thebrainquests9.pages.dev/category-destroy.html)
- [OFFICE PYRAMID SOLITAIRE](https://theeduplays-es.pages.dev/office-pyramid-solitaire.html)
- [CATEGORY ARENA255](https://thelearnquests9.pages.dev/category-arena255.html)
- [SPACE RAIDER](https://mindconvert.netlify.app/space-raider.html)
- [WORD RIVERS](https://jangkhangplay.pages.dev/word-rivers.html)
- [GRANNY 2 ASYLUM HORROR HOUSE](https://eduquestses.pages.dev/granny-2-asylum-horror-house.html)
- [IDLE MERGE CAR AND RACE](https://themindquests9.pages.dev/idle-merge-car-and-race.html)
- [CATEGORY CARE](https://eduquests.pages.dev/category-care.html)
- [WORLD Z DEFENSE ZOMBIE DEFENSE](https://eduquestses.pages.dev/world-z-defense-zombie-defense.html)
- [CATEGORY SHOOTER](https://theeduplays-es.pages.dev/category-shooter.html)
- [DICE MERGE](https://mindconvertpt.pages.dev/dice-merge.html)
- [I AM SECURITY](https://thelearnquests-ru.pages.dev/i-am-security.html)
- [EMERGENCY JAM](https://mindconvertes.pages.dev/emergency-jam.html)
- [SQUID GAME MEMORY CARD MATCH](https://thelearnquests9.pages.dev/squid-game-memory-card-match.html)
- [INDEX17](https://thelearningarcades.pages.dev/index17.html)
- [CATEGORY JUMP SCARE21](https://eduquests.pages.dev/category-jump-scare21.html)
- [ROBLOX HALLOWEEN COSTUME PARTY](https://thelearningarcades.pages.dev/roblox-halloween-costume-party.html)
- [21 CARDS](https://thestudyquests-ja.pages.dev/21-cards.html)
- [BLACK HOLE BEAUTY MAKEUP](https://mindconvertfr.pages.dev/black-hole-beauty-makeup.html)
- [SNOWBOARD GAME PARTY](https://thestudyarcades-vi.pages.dev/snowboard-game-party.html)
- [SHADOW STICKMAN FIGHT](https://brainquests.pages.dev/shadow-stickman-fight.html)
- [PIN DETECTIVE](https://thestudyquests-ja.pages.dev/pin-detective.html)
- [BEAT BLADER 3D](https://thelearnquests9.pages.dev/beat-blader-3d.html)
- [CATEGORY FPS 3](https://eduquestsfr.pages.dev/category-fps-3.html)
- [HOME ISLAND](https://thelearnquests9.pages.dev/home-island.html)
- [OVERPROTECTIVE BOYFRIEND](https://eduquests.pages.dev/overprotective-boyfriend.html)
- [IDLE AIRPORT CEO](https://mindconvertfr.pages.dev/idle-airport-ceo.html)
- [BEAUTY WORLD AND FASHION STYLIST](https://thestudyquests9.pages.dev/beauty-world-and-fashion-stylist.html)
- [FISH MASTER GO FISH](https://mindconvertfr.pages.dev/fish-master-go-fish.html)
- [FROGGY HOP](https://eduquestses.pages.dev/froggy-hop.html)
- [CATEGORY BIKE](https://jangkhangkr.pages.dev/category-bike.html)
- [GOLF ORBIT](https://thelearningarcades9.pages.dev/golf-orbit.html)
- [REAL PARKOUR SIMULATOR](https://theskillquests9.pages.dev/real-parkour-simulator.html)
- [LITTLE BUGS](https://jangkhangkr.pages.dev/little-bugs.html)
- [CROWD CLASH RUSH](https://themindinstitutes.pages.dev/crowd-clash-rush.html)
- [RABBIT CARROT](https://mindconvertfr.pages.dev/rabbit-carrot.html)
- [SECRETS OF CHARMLAND](https://theeduplays-es.pages.dev/secrets-of-charmland.html)
- [JEWEL LINK](https://mindconvert.netlify.app/jewel-link.html)
- [CATEGORY CONTROLLER 3](https://ieduquests.web.app/category-controller-3.html)
- [BUBBLE TROUBLE 2 REBUBBLED](https://brainquestsjp.pages.dev/bubble-trouble-2-rebubbled.html)
- [WAR ROBOTS BATTLES](https://thelearningarcades.pages.dev/war-robots-battles.html)
