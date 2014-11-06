Contributing to openage
=======================

- Before starting, [fork the repo and add the needed remotes](https://help.github.com/articles/fork-a-repo/). It's good ettiquete to start your work on a new branch called a *feature branch* that starts from the latest master (you can always switch to master and do `git pull upstream` (or the name of the remote you assigned to `SFTtech/openage` repo)) to update your local copy.

- Commit messages should be meaningful, they should say in a sentence (or very
little text) what changes it has without requiring to read the entire diff. [tpope knows this very well!](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)

- When making your first contribution, don't forget to add yourself to the authors list on `copying.md` :)

What makes a good Pull Request good?
---------------------------------

Before making a pull request, it's good to review these things:
- [Check your whitespaces](https://github.com/SFTtech/openage/blob/master/doc/code_style/tabs_n_spaces.md)
- [Read all the codestyle docs]( https://github.com/SFTtech/openage/tree/master/doc/code_style)
- Make sure that your code compiles!

After checking those up, if your development is done and you want to show your work (even if it has useless commits and such) you are ready to make the pull request.

The pull request will trigger many of the contributors to look at your code and maybe pointing out some things that you might haven't noticed, so here comes a process of iteration until a few of us are happy.

After the Almost Global Happiness™ is achieved, but before merging, you must do these things:
- **rebase**

- **squash** little commits into significant ones

- **push force** so you can keep your current pull request instead of spawning another one **WARNING: this is usually dangerous if you overwrite commits that other people may be using! use this only on your feature branches**


### wtf is rebase, squash or push force?

- **rebasing** is 'moving' your commits to the latest master, so you make sure that your changes apply even with recent code changes.

- **squashing** means "putting together many commits into a single one". We don't want to clutter the master log with commits like:
	- `initial try`
	- `oops it didnt work`
	- `yeahhh now it does`
	- `oops missing parenthesis`
	- *you go and post your PR, but TheJJ rants saying 'you asshole wrecked the whitespaces'*
	- `fix whitespace issues`
	- and so on...


- **push force** is the standard way of overwriting your development work with the fixed and mergeable version of your contribution.


### okay but how do i do it (doo bee doo bee do)
An easy way to do both **rebase** and **squash** is following these simple steps:
- `git checkout master`
- `git pull` to get the most recent changes (there might be new commits on master since you started coding!)
- `git checkout your-dev-branch` gets you back to bussiness
- `git checkout -b your-dev-branch-backup` so the next part isn't *that* scary
- `git checkout your-dev-branch` gets you back to bussiness again
- `git rebase -i master` <-- the (not) scary part

	With the `rebase -i` command (an interactive rebase) you can do many things:
	- squash your commits
	- mark a commit as fixup (like a silent squash, very useful)
	- reword a commit message
	- reorder commits

	(If you want an extra tutorial documentation about `rebasing` you can check [Atlasian's Git Tutorial](https://www.atlassian.com/git/tutorials/rewriting-history/), read the [Pro Git book](http://git-scm.com/book), look at the git-rebase manpage or google it)

As for **push force**, you can do it adding the `-f` flag to the same `git push` command that you use to upload the changes to your fork. This means overwriting your old development commits (maybe based on an old master) with the rebased+squashed ones.

After this you'll have in your branch and current PR the *Shiny, Squashed and Rebased Version™* ready (hopefully) to be merged.

### Congratulations!
