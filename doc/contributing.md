Contributing to openage
=======================

**"Can I even help?"**

You can't do anything wrong. Except not doing anything.

There is no wrongdoing, just wrong restraint.


What can I do?
--------------

**"Help - I want to contribute something, but I don't know what?"**

You're in luck. There's various sources for tasks:

 - Have a look at the [issue tracker](https://github.com/sfttech/openage/issues), especially issues with labels:
   - [good first issue](https://github.com/SFTtech/openage/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22)!
   - [just do it](https://github.com/SFTtech/openage/issues?q=is%3Aissue+is%3Aopen+label%3A%22just+do+it%22)!
 - Use [your favorite code searching tool](https://github.com/ggreer/the_silver_searcher) to find `TODO` messages!
 - The issue tracker [milestones](https://github.com/SFTtech/openage/milestones)!
 - [milestones.md](/doc/milestones.md) (more ambitious, obviously...)!
 - [Ask us](/README.md#contact)!


What's the concept?
-------------------

We basically do whatever we think is good to do.

The [openage architecture](/doc/architecture.md) is the foundation.
For our development concept, see the [development guide](/doc/development.md).

If you need some inspiration [what to work on, see above](#what-can-i-do).



**Please at least skip over this whole file if you want to contribute some code to *openage**!



tl;dr
-----

- Have a [GitHub](https://github.com) account
- Click the [fork button](https://github.com/SFTtech/openage)
- `git clone git@github.com:YourAccount/openage.git`
- `cd openage`
- `git remote add upstream https://github.com/SFTtech/openage.git`
- `git checkout -b tentacle-monster-fix`
- Edit file, adhere to [coding style](/doc/code_style)!
- Add yourself to `copying.md`
- `git add libopenage/unit/tentacle_monster.cpp`
- `git commit -m "engine: fixed vomiting animation of tentacle monster"`
- `make checkall`
- `make test`
- `git push origin tentacle-monster-fix`
- Create a pull request and look at the CI output

- Watch [cat pictures](https://www.flickr.com/search/?text=cat) while waiting for feedback


Read on below if you need more detailed instructions and quality hints.


Workflow
--------

We use the git fork/commit/pull request model.

Note: The following is for *larger features*.
For tiny stuff like typo fixes, just create your PR and be done with it.


- [Fork the repo and add the needed remotes](https://help.github.com/articles/fork-a-repo/).
  - the `upstream` remote is **SFTtech/openage**
  - the `origin` remote is **YourAccount/openage**
- Create a branch for your feature ("*feature branch*": `git checkout -b feature-name`).
  - This should only contain commits for `feature-name`,
    e.g. all changes relevant for branch `really-secure-drm`.
  - You can always update to upstream state by [rebasing](#rebasing).
- Discuss your ideas and your work:
  - On the [group chats](/README.md#contact)
  - That way, "bad ideas" can be "resolved" beforehand and "better ideas" are found

- "Release early and often!" also applies to pull requests!
  - Once your branch has some visible work, create `[WIP]` pull request
  - Give the pull request a description of what you did or want to do, so we can discuss it
  - Make sure you are in the `copying.md` file
  - People will be able to look at your code and give feedback
  - You'll get free checks from the build bot
- Once your work is done, remove the `[WIP]` so it can be merged
- Do the changes that are requested by the reviewers.
- Aaaaaand you're done.


What makes a good Pull Request good?
------------------------------------

Before making a pull request, it's good to review these things:
- Run `make test` to check whether any functionality has been broken
- [Check your whitespaces](https://github.com/SFTtech/openage/blob/master/doc/code_style/tabs_n_spaces.md)
- [Read all the codestyle docs]( https://github.com/SFTtech/openage/tree/master/doc/code_style)
- Before pushing, run `make checkall`. If that fails, the automatic buildbot will reject your code.
- If this is your first contribution, add yourself to the authors list in [copying.md](/copying.md).
- Commit messages should be meaningful, they should say in a sentence (or very little text) what
  changes it has without requiring to read the entire diff. [tpope knows this very well!](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)
- You should [rebase](#rebasing) your work to avoid "clutter" in your commits and use the latest upstream code.

We have a buildbot (currently [`kevin-ci`](https://github.com/SFTtech/kevin)) that runs all sorts of checks.
It can be a bit fascist at times, so don't be shocked if it rejects your code, and go fix it instead.


The pull request will present your code to the community, which may point out
some things that you might haven't noticed. You should fix stuff until everybody
is happy.


Rebasing
--------

**What the hell is it, and (why) do I need it?**

**Rebasing** is 'moving' your commits to a different parent commit.

In other words: *Cut off* your branch from its tree, and *attach it* somewhere else.

There's two main applications:

- If you based your work on a older master (so old that stuff can't be automatically merged),
  you can rebase to move your commits to the current [upstream](https://help.github.com/articles/fork-a-repo/) master:

```bash
# update the upstream remote to receive new commits
git fetch upstream

# be on your feature branch (you probably are)
git checkout nobody-likes-you

# make backup (you never know, you know?)
git branch nobody-likes-you-backup

# rebase: put your commits on top of upstream's master
git rebase -m upstream/master
```

- If you want to fix an older commit of yours, or merge several commits into a single one (**squash** them), rebase interactively.
  We ***don't*** want to have a commit history like this:

  - `add stuff`
  - `fix typo in stuff`
  - `fix compilation`
  - `change stuff a bit`
  - and so on...


### `rebase` in practice

`git log --graph --oneline` shows your commit history as graph.
To make some changes in that graph, you do an **interactive rebase**:

```
git rebase -i -m upstream/master
```

With this command, your new "base" is `upstream/master` and you can
then change any of your branch's commits.

`-i` will open an interactive editor where you can choose actions for each individual commit:

- re-order commits
- drop commits by deleting their line
- squash/fixup ("meld") your commits
- reword a commit message
- stop rebasing at a commit to edit (`--amend`) it manually

Just follow the messages on screen.


### Changing commits with `amend` and `fixup`

There's also `git commit --amend` which is a "mini-rebase" that modifies just the last commit with your current changes by `git add`.
It just skips the creation of a new commit and instead melds the changes into the last one you made.

If you want to update a single commit in the range `[upstream/master, current HEAD]` which is not the last commit:

- `edit stuff you wanna change in some previous commit`
- `git add changed_stuff`
- `git commit --fixup $hash_of_commit_to_be_fixed`
- `git rebase --autosquash -i -m upstream/master`


### Pushing changes

After you have rebased stuff (["rewritten history"](https://www.youtube.com/watch?v=9lXuZHkOoH8)) that had already been pushed,
git will not accept your pushes because they're not simple fast-forwards:

- The commit contents and the parent commit have changed as you updated the commit, therefore the commit hash changed, too.
  - If somebody used those commits, they will keep a copy
    and have a hard time updating to your updated version (because they "use" the old hashes).
  - Update your pull request branch with your re-written history!

- **force push** is the standard way of overwriting your development work with the fixed and mergeable version of your contribution!
  - Why? You changed the commits, so you want the old ones to be deleted!

  You can use any of:
  - `git push origin +nobody-likes-you`
  - `git push origin -f you-are-fat`
  - `git push origin --force and-you-are-adopted`

Some extra tutorials on `git rebase`:
 * [Atlassian's Git Tutorial](https://www.atlassian.com/git/tutorials/rewriting-history/)
 * [Pro Git book](http://git-scm.com/book)
 * `man git-rebase`
