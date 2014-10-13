openage tasks
=============

A mysterious voice told yourself:
"hey, myself, I want to contribute epic stuff to openage!"

But then your inner lazy piece of shit replied:
"but what should I do, I'm too lazy to think of something i could do!"


Suddenly, this document appears and presents you `task proposals`, and you
overcome your weaker self because of the awesomeness of these tasks.

If your inner voices are still not satisfied with these ideas and say:

* "Mkay, but I don't like these suggestions, they're crap!"
* "I don't know how to start, this is so complicated!"
* "Actually I was looking for granny porn when searching 'openage', what is this shit? computer-nerds?!?"

then don't hesitate to join the developer chats and ask for instructions:

* **IRC**: #sfttech on freenode.net

We will make fun of you at first (of course), but if you beg and promise us cake,
you might get a **custom special premium hand-crafted extra offer** what you could do,
and how.

If you've never programmed before, or are inexperienced:

0. This is the project you want to start your career with.
1. Seriously.
2. We'll give valuable feedback and try to support you where possible.
3. You might have guessed it, we will still make fun of you, though.


The [documentation tree description](doc/README.md) might be your first clue.

To get an overview about where to find what in the code,
take a look at [the project structure info documentation](doc/implementation/project_structure.md).


task proposals
--------------

Use `git describe` for seeing the latest milestone reached.

Look at [our roadmap](doc/milestones.md) to see what has to be achieved to
reach the next milestone. Ask in the **IRC** channel what's being worked on and
what may need help and support (even though it's known that nobody needs
that).


### NSA/BND interface
* before the secret services of the world subvert our codebase,
  we will be chivalrous and implement an official spying interface
* if we have an official interface, we can better say hi to our friends!
* no, not really, we actually should encrypt the game conversations

### free (as in freedom) assets
* we are depending on the proprietary original media files at the moment
* if you are a creative person, fire up your graphics software ([GIMP?](http://gimp.org/))
  and create replacement graphics
* free (creative commons licensed) media files liberate us from the evil leash
* reside in [openage-data repository](https://github.com/SFTtech/openage-data)
* original files will be used for free media files that are not present yet
* this means even drawing one building will help
* `png` images or better `svg` vectorgraphics

### inotify asset monitoring
* inotify is (if you don't know) a handy Linux kernel feature
  * monitors file modifications, creations, etc.
  * allows instaneous reaction to observed file updates without active polling
* when some cool guy/gal (might be you!) changes or creates media files,
  the change can instantly appear ingame
* very useful feature for people creating free media replacement files
* as this feature is Linux-specific, we have to make this feature "optional"
  -> preprocessor/cmake stuff
* implemented in `C++`

### game launcher
* startup program
* contains shiny buttons etc, we don't want to implement that in the game
* functions:
  * game lobby for joining multiplayer games
  * configure matches
  * select maps
  * launch the openage binary
* written in `Python` with *gtk3*

### lobby server
* server for mediate between game launchers
* maybe support for match-making
* could track player ranks and stats
* written in `Python` or `Haskell`

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
* you may implement this in `Python` or `C++`

### artificial intelligence
* live your wet dream and create our AI
* use machine learning, neural networks or whatever you like
* remember: the AI players get the same amount of information as humans,
  this means that e.g. undiscovered map areas won't be visible to the AI at all.
* the original AI scripting language could be integrated
* i'd propose to use `Python` for the AI scripting

### artificial intelligence interface
* the AI itself has to communicate with the game engine
* this interface provides the AI all the knowlege needed for proper operation
* exposes `C++` engine code to `Python`

### asset management frontend
* (preferrably) a GUI program to edit game assets
  * unit creation cost
  * tech tree editing
  * graphics and sounds
  * etc.
* similar to [Advanced Genie Editor](https://github.com/Tapsa/AGE)
* but compatible with the openage data formats
* maybe partially autogenerated by the conversion script and engine data format definitions
  * aviods redundancy on data format updates
  * allows to edit original game data as well!
* should use *gtk3*
* should be written in Python
