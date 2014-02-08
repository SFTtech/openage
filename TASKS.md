openage tasks
=============

a mysterious voice told yourself:
"hey, myself, I want to contribute epic stuff to openage!"

but then your inner evil replied:
"but what should I do, I'm too lazy to think of something i could do!"


suddenly, a wild JJ appears, and proposes `task proposals`,
and you inner evil instantly dies because of the awesomeness
of these tasks.

if your inner voices are still not satisfied with these ideas and say:
"mkay, I don't like these suggestions", or
"I don't know how to start, this is so complicated!", or even
"actually I was searching for granny porn by 'openage', what is this shit?",
don't hesitate to join the developer chats and ask for instructions.

* **IRC**: #sfttech on freenode.net
* **XMPP**: openage@chat.sft.mx

we will laugh at you at first (of course), but if you beg and promise us cake,
you might get a **special custom hand-crafted extra offer**
what you could do, and how.

if you've never programmed before,
this is the project you want to start your career with. seriously.
you might have guessed it, we will laugh at you as well, though.


to get an overview about where to find what in the code,
take a look at [the project structure info documentation](doc/implementation/project_structure).


task proposals
--------------

use `git describe` for seeing the latest milestone reached.

look at [our roadmap](doc/milestones) to see what has to be achieved to reach
the next milestone. ask in the IRC or XMPP channel what's being worked on.


### NSA interface
* before the secret services of the world subvert our codebase,
  we will be chivalrous and implement an official spying interface.
* if we have an official interface, we can better say hi to our friends!
* no, not really, we actually want to encrypt the game conversations.

### free (as in freedom) media data
* we are depending on the proprietary original media files at the moment
* free (creative commons licensed) media files liberate us from the evil leash
* reside in [openage-data repository](https://github.com/SFTtech/openage-data)
* original files will be used for free media files that are not present yet

### inotify monitoring
* inotify is (if you don't know) a handy Linux feature
  for monitoring file modifications, creations, etc.
* when some cool guy/gal (might be you!) changes or creates media files,
  the change can instantly appear ingame.
* as this feature is linux-specific, we have to make this feature "optional"
  -> preprocessor/cmake stuff
* will be implemented in C++

### game data storage
* come up with ideas on how to store all the game data
* our current plan:
  * csv files for metadata (how much does a castle cost?)
  * suitable folder structure for audio and image files
* the current storage location and format of files created
  by the convert script have to be improved
* this means you have to create a reasonable interface between
  the python convert script, and the C++ game
* this also means straight-forward integration of the free media files

### build system improvals
* create `make install` target
* make it possible to install openage-data
* allow external CXX and LDFLAGS
* improve the `./configure` script
* keep the `Makefile` in sync with the `cmake` config

### game launcher
* startup program
* contains shiny buttons etc, we don't want to implement that in the game
* functions:
  * game lobby for joining multiplayer games
  * configure matches
  * select maps
  * launch the openage binary
* written in Python with GTK3

### multilanguage support
* some easy and creative way of supporting localization
* should (of course) not impact performance
* as most translators are not steady for reading code,
  it should be easy to add new languages.
* i'd recommend using `gettext`

### random map generation
* you like the Minecraft terrain generation?
  you can implement it for openage.
* should be infinite-terrain-capable
  * map data for each chunk must be independently creatable
  * generation speed should be fast enough when chunks are created during game
* the original .rms scripts could be converted with python
* you may implement this in Python or C++

### path finding
* fast and efficient pathfinding for all unit movements
* communication with the obstruction management
* support for unit regrouping and intelligent blockage resolving
* has to be written in C++

### artificial intelligence
* live your wet dream and create our AI
* use machine learning, neural networks or whatever you like
* remember: the AI players get the same amount of information as humans,
  this means that e.g. undiscovered map areas won't be visible to the AI at all.
* the original AI scripting language could be integrated
* i'd propose to use Python for the AI scripting

### configuration system
* stores and reads plain-text config files
* manages all keybindings (hotkeys, mouse, ..)
* generic conffile support in the engine
* used by the game for the specific needs
* you could implement similar functionality like `engine/util/file.h::read_csv_file`
* should be written in C++
