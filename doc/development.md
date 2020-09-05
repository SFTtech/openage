About development
=================

Dev team
--------

There is a "core dev team" (*mic_e*, *JJ*, *zuntrax*, but there's more)
who work on the project in their free time,
but since we all have other life-originating interrupts and fluctuating motivation,
our activity will alternate equally over time (just take a look at the commit statistics).
We "core devs" have known each other personally for a long time;
thus, communication happens mostly over private channels (XMPP, Mumble, maybe even real life).


Development workflow
--------------------

Many contributions originate from discussions on the IRC channel,
or an issue on the [issue tracker](https://github.com/sfttech/openage/issues).

The pull requests are then open for review by everybody.
If the pull request author thinks that their goals aren't met yet,
it is tagged with `[WIP] $title`.
The author removes it once the goals are met.

In the big picture, we're working on completing those WIP pull requests,
user-submitted issues or the next milestone.
In reality however, we're often working on unrelated stuff that is not covered
by any milestone (buildsystem, infrastructure, ...),
just convenient to implement, or needed right now.

We're releasing early and often: whenever we add/improve a feature or
fix a bug and the code still compiles, we push the code to the github master.
Once significant changes have accumulated,
we tag that commit with a new minor version.

We don't have dedicated "subsystem maintainers" yet.
You can track down the person who did things with `git log` or `git blame`.


Contributing
------------

All sorts of contributions are welcome, including

 - feature requests
 - bug reports
 - code/doc typo fixes
 - code style fixes when our code doesn't conform to our own style guide (yes, that happens.)
 - bug fixes
 - new features
 - game assets under a free license
 - raytraced intro cutscenes
 - whatever else you can think of

But read [contributing.md](/doc/contributing.md) first to learn about the contribution workflow/requirements.

As always, join the [chat](/README.md#contact) and discuss your ideas.


Code philosophy
---------------

 - If upstream code (other parts of openage, other libraries,
   or even the compilers) doesn't work or is missing some feature,
   _report / fix it_! We contributed to compilers, cmake, Python, Cython and more because of openage.
 - Do things properly, not quickly. It takes research, time and maybe even upstream fixes, but it's worth it.
 - If you absolutely _must_ use a dirty hack (e.g. because you're waiting for upstream to fix it), write a TODO message with an explanation.
 - Regularly review old code to find rusty parts. Remove them, rewrite them, refactor them or at least update the comments.
 - The project is in development. Don't be shy about adding, removing and changing interfaces, but tell other people if you break their WIP stuff.
 - Others can't look into your brain. Document your stuff:
   - Explanations about inner workings algorithms and data structures should be comments in the source code.
   - Design decisions, API specification, rants about mathematical concepts and the like are explained in `doc/`.
