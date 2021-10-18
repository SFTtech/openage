# Emacs

[Emacs](https://www.gnu.org/software/emacs/) can be set up in infinitely many ways, depending on your needs and skills.

[Emacs](https://www.gnu.org/software/emacs/tour/) consists of a bare-metal core and many many addons written in [Emacs Lisp](https://en.wikipedia.org/wiki/Emacs_Lisp).
Some of the packages are built-in, others are in the [official repo](https://elpa.gnu.org/), and even more are fetched from [external repos](https://melpa.org).
Packages provide features and can modify each other.

Because plugging them together takes a lot of effort and basically behaves like your Linux distro just within Emacs, there's Emacs distributions.
To name a few:

* [Spacemacs](https://www.spacemacs.org/)
* [Doom](https://github.com/hlissner/doom-emacs)
* [Prelude](https://prelude.emacsredux.com)


## Spacemacs

> The best editor is neither Emacs nor Vim, it's Emacs and Vim!

Spacemacs is cloned to `~/.emacs.d` and provides the building blocks for a very well integrated Emacs IDE.

It supports three editing styles (and all available at once): Vim, Emacs and Spacemacs.

Probably best to start with [the quickstart guide](https://www.spacemacs.org/doc/QUICK_START.html).

[Below](#spacemacs-basics) is my rough introduction on how you can do things.


## JJ's setup

I'll try to present my setup in order to inspire your Emacs experience.

I use Spacemacs with [my custom ~/.spacemacs configuration](https://github.com/TheJJ/conffiles/blob/master/.spacemacs).
It's an all-in-one config file which configures and customizes Spacemacs.

The config enables and sets up several features, among them are:

* C++ and Python semantic autocompletion (with [lsp-mode](https://emacs-lsp.github.io/), [clangd](https://clangd.llvm.org/))
  * Symbol (function, class, variable, ...) reference and definition tracking
* Git project navigation and tracking ([projectile](https://projectile.mx/), [magit](https://magit.vc/))
  * Project compilation
  * Error highlighting
* Code snippet template expansion ([yasnippet](https://joaotavora.github.io/yasnippet/))
* Perfect codestyle indentation and formatting
  * Depending on the language: [Indent with tabs, align with spaces](/doc/code_style/tabs_n_spaces.md)
* More custom keybindings
* ...


In order for C++ semantic indexing to work with `openage`, symlink the `bin/compile_commands.json` to the project git root!
`clangd` needs to find this file in a parent dir of the source files so it knows the compiler invocations.

### Spacemacs Installation

* Distribution packages:
  * Install `emacs`, which is the GNU Emacs system package
  * Install `clangd` for semantic c++ parsing, usually part of `clang` distro package
  * Install `python-lsp-server` for semantic Python parsing
* Home directory configuration setup:
  * Clone Spacemacs: `git clone https://github.com/syl20bnr/spacemacs ~/.emacs.d`
  * Clone JJ's configs: `git clone https://github.com/TheJJ/conffiles ~/.jjconfigs`
  * Symlink the Spacemacs config: `ln -s .jjconfigs/.spacemacs ~/.spacemacs`
* Launch `emacs`
  * Wait until initial package setup is complete
  * Press <kbd>Space</kbd> to begin


### Spacemacs Updates

You need to **update** 4 things regularly:
* Emacs (your distro updates do this)
* Spacemacs (go to `.emacs.d` and do `git pull`)
* `.spacemacs`-config (go to `.jjconfigs` and do `git pull`, or whatever you use to track the `.spacemacs`)
* Emacs packages (click on `Update Packages` or press <kbd>SPC f e U</kbd> or run <kbd>M-x configuration-layer/update-packages</kbd>)


### Spacemacs Basics

You have three input methods active at once: **Vim**, **Emacs** *and* **Spacemacs**.

* If you're used to `vim`, most commands you're used to will probably just work.
* Most Emacs keybindings work too after you pressed `i` for the insert mode.
* Spacemacs bindings start by, who'd have thought it, pressing <kbd>Space</kbd>
* Press <kbd>ESC</kbd> to leave the current mode.
* Press <kbd>C-g</kbd> (control-g) **to abort whatever** input/action you're currently in.

But how can you do anything?

* I recommend you treat Emacs like Firefox - leave it open all the time
  * you can have many open tabs ("buffers") ([centaur for real tabs](https://develop.spacemacs.org/layers/+emacs/tabs/README.html))
  * you can have many open windows ("frames")
  * you can split the views ("windows")
  * open a new file in running Emacs from the commandline with: `emacsclient -n`
* Key combination syntax:
  * <kbd>C</kbd> = Control key
  * <kbd>M</kbd> = Meta = Alt key
  * <kbd>S</kbd> = Super = The key between <kbd>C</kbd> and <kbd>M</kbd>
  * <kbd>M-g f</kbd> = First press <kbd>Meta</kbd> and then <kbd>g</kbd>, let go of both, then press <kbd>f</kbd>
* Every key (and combination) is bound to a function invocation
  * You can invoke functions directly with <kbd>M-x</kbd>
    * What does a keypress do? <kbd>M-x describe-key</kbd>
    * Find definitions and documentation of a function: <kbd>M-x describe-function</kbd>
* Quit and Exit:
  * Spacemacs-style: <kbd>SPC q q</kbd>
  * Emacs-style: <kbd>C-x C-c</kbd>
  * Vim-style: <kbd>ESC :q</kbd>
* Buffer = opened real file (or for any other IO, like logs, ...)
  * Switch to buffer: <kbd>SPC b b</kbd> (<kbd>C-x b</kbd>)
  * Close buffer: <kbd>SPC b d</kbd> (<kbd>C-x k</kbd>)
* File Operations:
  * Open file browser: <kbd>SPC f f</kbd> (<kbd>C-x C-f</kbd>)
    * Open project: <kbd>SPC p p</kbd>
    * Open/switch to project file: <kbd>SPC p h</kbd>
  * Save: <kbd>SPC f s</kbd> (<kbd>C-x C-s</kbd>)
  * Close: <kbd>SPC b d</kbd> (<kbd>C-x k</kbd>)
  * Browse projects: <kbd>SPC p p</kbd>
* Window and views:
  * Split the view horizontally: <kbd>SPC w /</kbd> (<kbd>C-x 3</kbd>)
  * Close current view <kbd>SPC w d</kbd> (<kbd>C-x 0</kbd>)
  * Open a new Emacs window: <kbd>SPC F n</kbd> (<kbd>C-x 5 2</kbd>)
  * Close current Emacs window <kbd>SPC F d</kbd> (<kbd>C-x 5 0</kbd>)
  * Open the file browser tree: <kbd>SPC f t</kbd>
* Code naviation:
  * Go to line: <kbd>M-g g</kbd>
  * Find definition: <kbd>, g d</kbd> (<kbd>M-g d</kbd>)
  * Find references: <kbd>, g r</kbd> (<kbd>M-g f</kbd>)
  * Refactor variable name: <kbd>, r r</kbd>
  * Search the whole project code for something <kbd>M-x projectile-ag</kbd>
* Git status, commits, ...:
  * Launch Magit: <kbd>SPC g s</kbd>
  * View git-blame: <kbd>M-x vc-annotate</kbd>


An open buffer has several active modes:
* It's major-mode (e.g. `c++-mode`) and
* Minor modes (e.g. `line-number-mode`, `auto-completion`, ...; see them with <kbd>M-x describe-mode</kbd>).
* Each mode contributes to some behavior of the buffer (basically by providing/registering functions and setting variables).

This design allows you to fully customize and extend Emacs with whatever new feature and behavior you desire.


### Some features to start with

You can start these things with <kbd>M-x</kbd>, or hack them into `.spacemacs` once you get going with elisp.

* Classical menu bars: `menu-bar-mode`, `tool-bar-mode`
* Markdown live editing preview: `vmd-mode`
* Git: `magit-status`
* Remote over-ssh editing: `tramp` (just open file with name `/ssh:host:path/to/file`)
* Text Search: `ag`
* Debugging: `realgud`
* File tree: `treemacs`, `speedbar`
* Programming Symbol tree: `lsp-treemacs-symbols`
* Syntax check: `lsp-treemacs-error-list`, `flycheck-mode`
* Easy config customization: `customize`
* ...

Remember <kbd>M-x describe-$thing</kbd> to start digging into `$thing`!

### Your journey

Once you start missing features and wanting to customize the configuration further, please:
* either branch off my config repo to have your own `.spacemacs` and you can still merge my changes
* or start your own `.spacemacs` config!

Please [contact us](/#contact) if you need help or are overwhelmed by the awesomeness!
