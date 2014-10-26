Openage tasks
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


Task proposals
--------------

Use `git describe` for seeing the latest milestone reached.

Look at [our roadmap](milestones.md) to see what has to be achieved to
reach the next milestone. Ask in the **IRC** channel what's being worked on and
what may need help and support (even though it's known that nobody needs
that).


### NSA/BND interface
* Before the secret services of the world subvert our codebase,
  we will be chivalrous and implement an official spying interface
* If we have an official interface, we can better say hi to our friends!
* No, not really, we actually should encrypt the game conversations

### Free (as in freedom) assets
* We are depending on the proprietary original media files at the moment
* If you are a creative person, fire up your graphics software ([GIMP?](http://gimp.org/))
  and create replacement graphics
* Free (creative commons licensed) media files liberate us from the evil leash
* Reside in [openage-data repository](https://github.com/SFTtech/openage-data)
* Original files will be used for free media files that are not present yet
* This means even drawing one building will help
* `png` images or better `svg` vectorgraphics

### Inotify asset monitoring
* Inotify is (if you don't know) a handy Linux kernel feature
  * Monitors file modifications, creations, etc.
  * Allows instaneous reaction to observed file updates without active polling
* When some cool guy/gal (might be you!) changes or creates media files,
  the change can instantly appear ingame
* Very useful feature for people creating free media replacement files
* As this feature is Linux-specific, we have to make this feature "optional"
  -> preprocessor/cmake stuff
* Implemented in `C++`

### Game launcher
* Startup program
* Contains shiny buttons etc, we don't want to implement that in the game
* Functions:
  * Game lobby for joining multiplayer games
  * Configure matches
  * Select maps
  * Launch the openage binary
* Written in `Python` with *gtk3*

### Lobby server
* Server for mediate between game launchers
* Maybe support for match-making
* Could track player ranks and stats
* Written in `Python` or `Haskell`

### Multilanguage support
* Some easy and creative way of supporting localization
* Should (of course) not impact performance
* As most translators are not steady for reading code,
  it should be easy to add new languages.
* I'd recommend using `gettext`

### Random map generation
* You like the Minecraft terrain generation?
  You can implement it for openage.
* Should be infinite-terrain-capable
  * Map data for each chunk must be independently creatable
  * Generation speed should be fast enough when chunks are created during game
* The original .rms scripts could be converted with python
* You may implement this in `Python` or `C++`

### Artificial intelligence
* Live your wet dream and create our AI
* Use machine learning, neural networks or whatever you like
* Remember: The AI players get the same amount of information as humans,
  this means that e.g. undiscovered map areas won't be visible to the AI at all.
* The original AI scripting language could be integrated
* I'd propose to use `Python` for the AI scripting

### Artificial intelligence interface
* The AI itself has to communicate with the game engine
* This interface provides the AI all the knowlege needed for proper operation
* Exposes `C++` engine code to `Python`

### Asset management frontend
* (Preferrably) a GUI program to edit game assets
  * Unit creation cost
  * Tech tree editing
  * Graphics and sounds
  * Etc.
* Similar to [Advanced Genie Editor](https://github.com/Tapsa/AGE)
* But compatible with the openage data formats
* Maybe partially autogenerated by the conversion script and engine data format definitions as this:
  * Aviods redundancy on data format updates
  * Allows to edit original game data as well!
* Should use *gtk3*
* Should be written in Python
