Contributing to openage
=======================

"Can I even help?"
------------------

You can't do anything wrong. Except not doing anything.

There is no wrongdoing, just wrong restraint.


"Help - I want to contribute something, but I don't know what?"
---------------------------------------------------------------

You're in luck. There's various sources for tasks:

 - have a look at the [issue tracker](https://github.com/sfttech/openage/issues), especially at the issues labeled [easy](https://github.com/SFTtech/openage/labels/easy) or [just do it](https://github.com/SFTtech/openage/labels/just%20do%20it)
 - use [your favourite code searching tool](https://github.com/ggreer/the_silver_searcher) to find `TODO` messages (there's quite a bunch of them)
 - [tasks.md](/doc/tasks.md)
 - [milestones.md](/doc/milestones.md) (more ambitious, obviously...)
 - ask on IRC


tl;dr
-----

- Have a [GitHub](https://github.com) account
- Click the [fork button](https://github.com/SFTtech/openage)
- `git clone git@github.com:YourUsername/openage.git`
- `cd openage`
- `git remote add upstream https://github.com/SFTtech/openage.git`
- `git checkout -b tentacle-monster-fix`
- Edit file, adhere to [coding style](/doc/code_style)!
- Add yourself to `copying.md`
- `git add libopenage/tentacle_monster.cpp`
- `git commit -m "engine: fixed vomiting speed of tentacle monster"`
- `make checkall`
- `make test`
- `git push origin tentacle-monster-fix`
- Create a pull request and look at the CI output

- Watch [cat pictures](https://www.flickr.com/search/?text=cat) while waiting for feedback


Read on below if you need more detailed instructions and quality hints.


Workflow
--------

Please at least skip over this whole file if you want to contribute some code to **openage**.

We use Github's fork/commit/pull request model.

- [Fork the repo and add the needed remotes](https://help.github.com/articles/fork-a-repo/).
  - the `upstream` remote is **SFTtech/openage**
  - the `origin` remote is **YourMom/openage**
- Create a branch for your feature (*feature branch*: `git checkout -b feature-name`).
  - This should only contain commits for `feature-name`, e.g. changes relevant for branch `fix-mom-mass`.
  - You can always switch to master and do `git pull upstream master` and rebase (see below) to update your local copy.
- Discuss your ideas and your work
  - On the IRC channel (ask for opinions or at least announce it)
  - This prevents those "duplicate work" situations where two people didn't know of each other's ongoing work.
    "Release early and often!" also applies to pull requests!
- Once your branch has some visible work, create "[WIP]" pull request
  - Prefix your pull request's name with "[WIP] ").
  - Make sure you are in the `copying.md` file.
  - Entire dev community can see that there's something going on here.
  - People will be able to look at your code (e.g. to see whether that would break the stuff they are doing),
    and you'll get free checks from the build bot.
- Finally, once your work is done and you think it's ready to be merged,
  rename your pull request to remove the "[WIP]" (and announce that fact on the IRC, or it may go unnoticed).
- Do the changes that are requested by the reviewers.
- Aaaaaand you're done.

Note: All of this is for larger features. For tiny stuff like typo fixes, just create your PR and be done with it.


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
- If you based your work on a heavily outdated master or you have lots of "clutter" in your history, see the section about rebasing below.

We have a buildbot (currently `kevin-ci`) that runs all sorts of checks.
It can be a bit fascist at times, so don't be shocked if it rejects your code, and go fix it instead.

After checking those up, your development is done and you want to show your work
(even if it has useless commits and such) you are ready to make the pull request.

The pull request will trigger many of the contributors to look at your code
and maybe pointing out some things that you might haven't noticed,
so there will be an iterative process where we tell you to fix stuff until everybody is happy.


Rebasing - What the hell is it, and (why) do I need it?
-------------------------------------------------------

**rebasing** is 'moving' your commits to a different base commit. There's two main applications:

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

 - `foo`
 - `initial try of bar`
 - `oops it didnt work`
 - `yeahhh now it does`
 - `oops missing parenthesis`
 - `fix whitespace issues`
 - `oops there was a typo in the docs for foo`
 - and so on...

 `git log` shows your commit history, to make some changes, you do an **interactive rebase**:

 `git rebase -i -m upstream/master` lets you rewrite the history of all your commits down to your fetched upstream master branch.
 `rebase -i` will open an interactive editor where you can choose actions for each individual commit:

 - re-order commits
 - drop commits by deleting their line
 - squash your commits
 - mark a commit as fixup (like a silent squash, very useful)
 - reword a commit message
 - stop rebasing at a commit to edit (`--amend`) it manually

 There's also `git commit --amend` which is a "mini-rebase" that modifies just the last commit with your changes.
 This just skips the creation of a new commit and instead melds the changes into the last one you made.

 If you want to update a single commit in the range `[upstream/master, current HEAD]` which is not the last commit:

 - `edit stuff you wanna change in some previous commit`
 - `git add changed_stuff`
 - `git commit --fixup $hash_of_commit_to_be_fixed`
 - `git rebase --autosquash -i -m upstream/master`

- After you have rebased stuff (["rewritten history"](https://www.youtube.com/watch?v=9lXuZHkOoH8)) that had already been pushed,
  git will not accept your pushes because they're not simple fast-forwards:
 - The commit contents have changed as you updated the commit, therefore the commit hash changed, too.
 - Other people who also work on that same branch will have their history broken (because they "use" the old hashes).
 - But this is your pull request, so you're the only one working on it.
 - If you change history for some branch others depend on, they'll hate you forever, because their branch base is now gone.
 - Use **push -f** to place your re-written history on a pull request branch.

- **push force** is the standard way of overwriting your development work with the fixed and mergeable version of your contribution
 - you can use any of:
  - `git push origin +nobody-likes-you`
  - `git push origin -f you-are-fat`
  - `git push origin --force and-you-are-adopted`

Some extra tutorials on `rebase`: [Atlasian's Git Tutorial](https://www.atlassian.com/git/tutorials/rewriting-history/), the [Pro Git book](http://git-scm.com/book), `man git-rebase`.
