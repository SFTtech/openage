# Procedure for Microsoft Windows users (Windows 10)

 Since Windows doesn't offer a native package manager, we use a mixture of manual and automated steps to get the dependencies for openage. *Please remember to replace the directories referenced below (written in <...>) with the appropriate values.*

## Setting up the build environment
 You will need to download and install the following manually. Those who already have the latest stable versions of these softwares can skip this:
 - [Visual Studio 2017 Community edition](https://www.visualstudio.com/downloads/)
   - With the "Desktop development with C++" workload.
   - With the "Windows 10 SDK". Choose the latest version listed.
 - [Python 3](https://www.python.org/downloads/windows/)
   - With the "pip" option enabled. We use `pip` to install other dependencies.
   - With the "Precompile standard library" option enabled.
   - With the "Download debug binaries (...)" option enabled.
   - If in doubt, run the installer again and choose "Modify".
 - [CMake](https://cmake.org/download/)

### Python Modules
 Open a command prompt at `<Python 3 installation directory>/Scripts`

    pip install cython numpy pillow pygments pyreadline

### vcpkg packages
 Set up [vcpkg](https://github.com/Microsoft/vcpkg#quick-start). Open a command prompt at `<vcpkg directory>`

    vcpkg install dirent libepoxy fontconfig freetype harfbuzz opus opusfile qt5 sdl2 sdl2-image libpng

 _Note:_ Building and installing `qt5` using vcpkg takes a lot of space-time. You might want to install [the prebuilt version](https://www.qt.io/download-open-source/) instead. This should be mentioned in `CMAKE_PREFIX_PATH` with the cmake command below.

### The "other missing" dependencies
 **opus-tools (for `opusenc`)**

  There are two options for getting opus-tools:

  [Get the prebuilt binaries](https://archive.mozilla.org/pub/opus/win32/).

  OR

  Build it yourself.
  You can get the source from [the Github page](https://github.com/xiph/opus-tools).
  Refer their AppVeyor config for the steps to build.
  You will need `opusenc.exe` from the build output directory available from the `PATH`.

## Building openage
 Note that openage doesn't support completely out-of-source-tree builds yet.
 We will, however, use a separate `build` directory to build the binaries.

 Open a command prompt at `<openage directory>`:

     mkdir build
     cd build
     cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg directory>/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
     cmake --build . --config RelWithDebInfo -- /nologo /m /v:m

## Running openage (in devmode)
 While this is straightforward on other platforms, there is still stuff to do to run openage on Windows:
  - Install the [DejaVu Book Font](https://dejavu-fonts.github.io/Download.html).
    - Set the `FONTCONFIG_PATH` environment variable to `<vcpkg directory>\installed\<relevant config>\tools\fontconfig\fonts\conf.d`.
    - Copy `fontconfig/57-dejavu-serif.conf` to the same.
  - [Optional] Set the `AGE2DIR` environment variable to the AoE 2 installation directory.
  - Copy the binaries from the `build` subdirectories to the required locations:

        set ConfigToUse=RelWithDebInfo

        copy libopenage\%ConfigToUse%\openage.dll ..\
        copy libopenage\%ConfigToUse%\openage.pdb ..\
  - Append the following to the environment `PATH`:
    - `<vcpkg directory>\installed\<relevant config>\bin`
    - Path to `opusenc.exe`

 Now, execute `<openage directory>/run.exe` and enjoy!

 ## Packaging
 Open a command prompt at `<openage directory>\build` (or use the one from the building step):

    cpack -C RelWithDebInfo

 The installer (`openage-<version>-win32.exe`) will be generated in the same directory.<br>
 _Hint_: Append `-V` to the `cpack` command for verbose output (it takes time to package all dependencies).
