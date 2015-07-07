About development
=================

Dev team
--------

There is a "core dev team" (mostly *mic_e* and *JJ*, but there's more) who work on the project in their free time, but since we all have real jobs [0] and fluctuating motivation, our activity will alternate equally over time (just take a look at the Github commit stats).
We "core devs" have known each other personally for a long time; thus, communication happens mostly over private channels (XMPP, Mumble, maybe even real life).

Core development workflow
-------------------------

All new features or things of importance are discussed on the IRC channel, or an issue is created on the [issue tracker](https://github.com/sfttech/openage/issues). Larger new contributions are announced as a *WIP* pull request. Before they're merged, the PRs are open for review by everybody.
In the big picture, we're working on completing those WIP pull requests, user-submitted issues or the next milestone. In reality however, we're often working on unrelated stuff that is not covered by any milestone (buildsystem, infrastructure, ...), just convenient to implement, or needed right now.

We're releasing early and often; whenever we add/improve a feature or fix a bug, and the game still compiles, we push the code to the github master. Once significant changes have accumulated, we tag that commit with a new minor version. The major version is the current milestone number ([milestones.md](milestones.md)).

No need to have dedicated "subsystem maintainers" has arisen yet; you can figure out the responsibilities from the git log or git blame.

Contributing
------------

All sorts of contributions are welcome, including

 - feature requests
 - bug reports
 - code/doc typo fixes
 - code style fixes when our code doesn't conform to our own style guide (yes, that happens.)
 - bug fixes
 - new features
 - raytraced intro cutscenes
 - whatever else you can think of

But read [contributing.md](/doc/contributing.md) first to learn about the contribution workflow/requirements.

We'd also be happy about adding new core developers to the team. You should speak passable English, be skilled in Python or C++ and _not_ be a Microsoft fanboy, but apart from that, everybody is welcome!
Once you've contributed a significant amount of non-horrible code, you'll receive *trusted core dev*™ status and receive direct push access; this will be decided by a multi-headed committee during a week-long conference [1].

As always, join the IRC channel (`freenode.net#sfttech`) and ping one of the devs directly if you have questions about contributing.

We operate on a four-eyes policy: Major pull requests from one core developer need to be reviewed by at least one other core developer, and other major pull requests need to be reviewed by at least two core developers. Reviews from other devs are always welcome (and will accelerate your ascension to *trusted core dev*™).

Code philosophy
---------------

 - If upstream code (other parts of openage, other libraries, or even the compilers) doesn't work or is missing some feature, _report / fix it_.
 - Do things properly, not quickly. It takes research, time and maybe even upstream fixes, but it's worth it.
 - If you absolutely _must_ use a dirty hack (e.g. because you're waiting for upstream to fix it), write a TO-DO message with an explaination.
 - Regularily review old code to find rusty parts. Remove them, rewrite them, refactor them or at least update the comments.
 - The project is in development. Don't be shy about adding, removing and changing interfaces, but tell other people if you break their WIP stuff.

Dev setup
---------

We're all on Linux (with Intel/NVidia GPUs), and nobody uses any IDEs (unless you count `emacs`). If you want to use an IDE, first of all reconsider: Neither C++ nor Python are IDE-centric languages (unlike Java or C#) and may not profit from an IDE.
If you managed to get the build system and everything working with your favourite IDE, please contribute instructions to the docs.

[0] actually we don't; we're students.
[1] also known as a LAN party. You're invited.
