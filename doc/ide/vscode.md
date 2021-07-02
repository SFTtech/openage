# Visual Studio Code

vscode can be used to launch build, edit, run and debug C++ code.

There are two different versions of vscode that you can use:

* The official *vscode* release from Microsoft for which you can get an installer on the the Visual Studio Code website.
* The inofficial fork *vscodium* that removes all proprietary components and disables telemetry. You can find it [here](https://vscodium.com/).


## Setting up vscode for openage

It is a good idea to install extensions that enable language support for the programming languages used in the project. The languages we use can be found in the general [README](/README.md#technical-foundation) file.

Add extensions to vscode by going to **File** -> **Preferences** -> **Extensions**.

If you use *vscodium*, some extensions might not be available from the start and have to be added manaually. Read more on this in the project's [documentation](https://github.com/VSCodium/vscodium/blob/master/DOCS.md#extensions-marketplace).

If you use the official *vsode* release, now would be a good time to disable telemetry. To do this, go to **File** -> **Preferences** -> **Online Services Settings** and disable the telemetry options.


## Opening and configuring the openage workspace

Creating a workspace for openage is very straightforward:

1. Clone the *openage* repository: https://github.com/SFTtech/openage/
1. Install all openage dependencies for your platform: https://github.com/SFTtech/openage/blob/master/doc/building.md#dependency-installation
1. Clone the *nyan* repository: https://github.com/SFTtech/nyan
1. Install all nyan dependencies for your platform: https://github.com/SFTtech/nyan/blob/master/doc/building.md#dependencies
1. Go to **File** -> **Open Folder...** and select the root folder of the openage repository

Afterwards, vscode will try to initialize your workspace with your installed extensions. Once that is finished, you should change a few settings of the default settings in your workspace.

**Files: Watcher Exclude**

You should add these entries to the list of directories that are not watched:

* `**/.bin/**`
* `**/bin/**`
* `**/assets/converted/**`

These directories contain generated build files, binaries created from builds and media assets converted from the original games. Hence, there is no need to watch these files for changes during development. If you do not add these entries, vscode will likely complain that there are too many files for it to index.

**C_Cpp> Default: Include Path**

If you use a C/C++ language support extension, you need to add the path to the *nyan* directory to your workspace's include path. Otherwise, vscode will not be able to find the nyan header files.

1. Select **Edit in settings.json**
1. Scroll down and add a new parameter entry to the end of the JSON object. This will enable custom include paths.

```
"C_Cpp.default.includePath": [
    "${default}"
]
```

1. Open **c_cpp_properties.json** in the same folder and add a new entry to `includePath` containing the path to the nyan repository on your file system. If your local nyan repository is in the same folder as the openage repository, your entry should look like this:

```
"includePath": [
    "${workspaceFolder}/**",
    "${workspaceFolder}/../nyan/**"
]
```

## Add openage configure and build tasks

vscode will likely automatically generate a build configuration for CMake when you create the workspace, but it's not recommended to use that. Instead, you should always use our official build configuration using the `configure` script that comes with the project. To do this, you can use our template task file that you can find at `/doc/ide/configs/vscode/tasks.json` in the openage repository files. Copy this file to the `/.vscode/` directory to make the tasks available in your workspace.

Task                | Description
--------------------|-------------
`Configure & Build` | Shortcut to run the `Configure` and `Build` tasks consecutively.
`Configure`         | Starts the CMake configuration, i.e. runs the `configure` script.
`Build`             | Starts the CMake build, i.e. runs `make`.
`Sanity Check`      | Checks for code compliance errors. You need to fix the complaints before your PR can be merged.
`Run Tests`         | Run all test for the engine modules.
`Clean`             | Clean all build directories by removing all build files.
