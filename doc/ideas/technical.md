# Technical Ideas

## Monitor Game Asset Folder (e.g. with inotify)

  - when the user modifies an asset, such as a texture, the in-game models are updated instantly
    - awesome live-testing for modders
  - far more challenging: make this work for unit stats.


## Built-in Snapshot Creation for Mods

Let modders make a snapshot of their current build via a provided engine tool. The mod
can be rolled back to a previous state with this feature. Snapshots could be incremental
(only changes to previous builds are saved) or fully-featured (the snapshot itself works
like a standalone build).

Incremental snapshots could make use of a version control system like git or SVN, although
this would be limited to non-binary files only. It would require a git wrapper in the editor
and the modding tools.

Snapshots could contain information that are unnecessary for normal builds such as debug
information or generated data from test cases.
