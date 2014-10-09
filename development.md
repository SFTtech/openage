about development
=================

There is a "core dev team" of a few people who work on the project in their free time, but since we all have real jobs [0] and fluctuating motivation, our activity will alternate equally over time (just take a look at the Github commit stats).
All "core devs" have known each other personally for a long time; thus, communication happens mostly over private channels (XMPP, Mumble, maybe even real life).

**However**, we (especially *JJ*) are *always* looking for new contributions of any kind. That includes bug reports and feature requests (see the [issue tracker](https://github.com/sfttech/openage/issues), which is, at the time of writing, unfortunately pretty empty), but actual commits (grammar fixes in the docs, code contributions, whatever...) are even better.
But we'd be most happy about new devs joining our core team. You should speak passable English, be skilled in Python or C++ and _not_ be a Microsoft fanboy, but apart from that everybody is welcome! Just contact us and I'm sure someone will give you an introduction into the code.

For anything (including contributions), the best way of contacting us is to join our IRC channel (`freenode.net#sfttech`) and ping one of the devs directly.
The best way of contributing commits is probably to fork the project on Gibhub and send a pull request via their web interface, but... you're free to choose any other format, as long as we can figure out how to merge it. You'll get direct push permissions to all our repos once you're a "trusted core dev" (this is decided by a multi-headed committee during a week-long conference[1]).

We're always releasing early and often; whenever we add/improve a feature or fix a bug, and the game still compiles, we push the code to the github master. Once significant changes have accumulated, we tag that commit with a new minor version. The major version is the current milestone number ([doc/milestones.md](doc/milestones.md)).

In the big picture, we're always working towards the next milestone. In reality however, we're often working on unrelated stuff that is either not covered by any milestone (build system, ...) or covered by a later milestone but convenient to implemented, or neccesary, right now.

We're all on Linux (with Intel/NVidia GPUs), and nobody uses any IDEs (unless you count `emacs`). If you want to use an IDE, first of all reconsider: Neither C++ nor Python are IDE-centric languages (unlike Java or C#)

[0] actually we don't; we're students.
[1] also known as a LAN party. You're invited.
