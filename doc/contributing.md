Contributing to openage
=======================

"Help - I want to contribute something, but I don't know what?"
---------------------------------------------------------------

You're in luck. There's various sources for tasks:

 - have a look at the [issue tracker](https://github.com/sfttech/openage/issues), especially at the issues labeled [easy](https://github.com/SFTtech/openage/labels/easy) or [just do it](https://github.com/SFTtech/openage/labels/just%20do%20it)
 - use [your favourite code searching tool](https://github.com/ggreer/the_silver_searcher) to find `TODO` messages (there's quite a bunch of them)
 - [tasks.md](/doc/tasks.md)
 - [milestones.md](/doc/milestones.md) (more ambitious, obviously...)
 - ask on IRC


Workflow
--------

Please at least skip over this whole file if you want to contribute some code to openage.

We use Github's fork/commit/pull request model.

- [Fork the repo and add the needed remotes](https://help.github.com/articles/fork-a-repo/).
- Create a branch for your feature (*feature branch*). You can always switch to master and do `git pull upstream` and rebase (see below) to update your local copy.
- Discuss your ideas and your work, e.g. on the IRC channel (ask for opinions or at least announce it), to prevent those "duplicate work" situations where two people didn't know of each other's ongoing work ("release early and often!" also applies to pull requests).
- Once you have commited some work to your feature branch, open a "[WIP]" pull request (prefix your pull request's name with "[WIP] "). That will make it clear to the entire dev community that there's something going on here. People will be able to look at your code (e.g. to see whether that would break the stuff they are doing), and you'll get free checks from the build bot.
- Finally, once your work is done and you think it's ready to be merged, rename your pull request to remove the "[WIP]" (and announce that fact on the IRC, or it may go unnoticed).
- Do the changes that are requested by the reviewers.
- Aaaaaand you're done.

Note: All of this is for larger features. For tiny stuff like typo fixes, just create your PR and be done with it.


What makes a good Pull Request good?
------------------------------------

Before making a pull request, it's good to review these things:
- run `make test` to check whether any functionality has been broken
- [Check your whitespaces](https://github.com/SFTtech/openage/blob/master/doc/code_style/tabs_n_spaces.md)
- [Read all the codestyle docs]( https://github.com/SFTtech/openage/tree/master/doc/code_style)
- before pushing, run `make checkall`. If that fails, the automatic buildbot will reject your code.
- if this is your first contribution, add yourself to the authors list in [copying.md](/copying.md).
- Commit messages should be meaningful, they should say in a sentence (or very little text) what changes it has without requiring to read the entire diff. [tpope knows this very well!](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)
- If you based your work on a heavily outdated master or you have lots of "clutter" in your history, see the section about rebasing below.

We have a buildbot (currently `travis-ci`) that runs all sorts of checks; it can be a bit fascist at times, so don't be shocked if it rejects your code, and go fix it instead.

After checking those up, if your development is done and you want to show your work (even if it has useless commits and such) you are ready to make the pull request.

The pull request will trigger many of the contributors to look at your code and maybe pointing out some things that you might haven't noticed, so there will be an iterative process where we tell you to fix stuff until everybody is happy.


Rebasing - What the hell is it, and (why) do I need it?
-------------------------------------------------------

**rebasing** is 'moving' your commits to a different base commit. There's two main applications:

- If you based your work on a very old master (so old that stuff can't be automatically merged), you can rebase to move your commits to the current master:

```
# update master
git checkout master
git pull
git checkout my_feature_branch
# make backup (you never know, you know?)
git checkout -b my_feature_branch-backup
get checkout my_feature_branch
# rebase
git rebase master
```

- If you want to fix an older commit of yours, or merge several commits into a single one (**squash** them), rebase interactively. We don't want to have a commit history like this:

 - `foo`
 - `initial try of bar`
 - `oops it didnt work`
 - `yeahhh now it does`
 - `oops missing parenthesis`
 - `fix whitespace issues`
 - `oops there was a typo in the docs for foo`
 - and so on...

  `git log` shows your commit history, and `git rebase -i master` modifies the history of all your commits down to your local version of `master`. `rebase -i` will open an interactive editor where you can choose actions for each individual commit:
 - squash your commits
 - mark a commit as fixup (like a silent squash, very useful)
 - reword a commit message
 - re-order commits
 - stop rebasing to fix some files manually

 There's also `git commit --amend` which is a "mini-rebase" that modifies just the previous commit.

- After you have rebased stuff (["rewritten history"](https://www.youtube.com/watch?v=9lXuZHkOoH8)) that had already been pushed, Github will not accept your pushes because they're not simple fast-forwards; other people who also work on that branch will have their history broken, but this is your pull request, so you're the only one working on it. Use **push -f** to place your re-written history on Github.

- **push force** is the standard way of overwriting your development work with the fixed and mergeable version of your contribution.

Some extra tutorials on `rebase`: [Atlasian's Git Tutorial](https://www.atlassian.com/git/tutorials/rewriting-history/), the [Pro Git book](http://git-scm.com/book), `man git-rebase`.
