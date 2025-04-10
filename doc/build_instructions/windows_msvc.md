# Procedure for Microsoft Windows users

<!---
__NOTE:__ We also have an installer for Win10 (x64), if you just want to play around with *openage* you can find it [here](https://github.com/SFTtech/openage/releases).

 Since Windows doesn't offer a native package manager, we use a mixture of manual and automated steps to get the dependencies for openage.
 *Please remember to replace the directories referenced below (written in <...>) with the appropriate values.*

## Using CI to build openage
If you use any CI (like Travis-CI or Appveyor) you can make your life easier by using the following yaml-configuration files:
- Win_x64 - MSVC 15.x - [> Download .yml <](https://gist.githubusercontent.com/simonsan/4c73314e005239938110ec9c91e484c0/raw/)
- Win_x86 - MSVC 15.x - [> Download .yml <](https://gist.githubusercontent.com/simonsan/390f2e3f60667608f74a2ed687e14dad/raw/)

They will build you the latest version from our master branch and package them into an installer and a portable 7z-file.

__NOTE:__ You need to manually make sure and doublecheck if the system you are building on has fulfilled all the [dependencies](/doc/building.md).
-->

## Setting up the build environment
You will need to download and install the following manually.
Those who already have the latest stable versions of these programs can skip this:

- [Visual Studio Buildtools](https://aka.ms/vs/17/release/vs_BuildTools.exe)
  - With the "Visual C++ Buildtools" workload.
  _NOTE:_ If you are searching for an IDE for development you can get an overview [here](https://en.wikipedia.org/wiki/Comparison_of_integrated_development_environments#C/C++). We've also written some [instructions for developing with different IDEs](/doc/ide/README.md).

- [Python 3](https://www.python.org/downloads/windows/)
  - With the *pip* option enabled. We use `pip` to install other dependencies.
  - With the *Precompile standard library* option enabled.
  - With the *Download debug binaries (...)* option enabled.
  - If in doubt, run the installer again and choose *Modify*.
  - You are going to need the 64-bit version of python if you are planning to build the 64-bit version of openage, and vice versa.

 - [CMake](https://cmake.org/download/)

### Python Modules
Open a command prompt at `<Python 3 installation directory>/Scripts`

```ps
pip install cython numpy lz4 toml pillow pygments pyreadline3 mako
```
_Note:_ Make sure the Python 3 instance you're installing these scripts for is the one you call `python` in CMD

_Note:_ Also ensure that `python` and `python3` both point to the correct and the same version of Python 3

### vcpkg packages
Set up [vcpkg](https://github.com/Microsoft/vcpkg#quick-start). Open a command prompt at `<vcpkg directory>`

```ps
vcpkg install dirent eigen3 fontconfig freetype harfbuzz libepoxy libogg libpng opus opusfile qtbase qtdeclarative qtmultimedia toml11
```

If you also want Vulkan graphics support, you should also run this command:

```ps
vcpkg install vulkan
```

_Note:_ The `qt` port in vcpkg has been split into multiple packages, build times are acceptable now. If you want, you can still use [the prebuilt version](https://www.qt.io/download-open-source/) instead. If you do so, include `-DCMAKE_PREFIX_PATH=<QT6 directory>` in the cmake configure command.

_Note:_ If you are planning to build the 64-bit version of openage, you are going to need 64-bit libraries. Add command line option `--triplet x64-windows` to the above command or add the environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows` to build x64 libraries. [See here](https://github.com/Microsoft/vcpkg/issues/1254)

<!---
__NOTE:__ You can also download the pre-built vcpkg dependencies (without Qt) [from this repository](https://github.com/simonsan/openage-win-dependencies/releases).
-->

## Building openage
Note that openage doesn't support completely out-of-source-tree builds yet. We will, however, use a separate `build` directory to build the binaries.

_Note:_ You will also need to set up [the dependencies for nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md#windows), which is mainly [flex](https://sourceforge.net/projects/winflexbison/).

Open a command prompt at `<openage directory>`:

```ps
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg directory>\scripts\buildsystems\vcpkg.cmake ..
cmake --build . --config RelWithDebInfo -- /nologo /m /v:m
```

_Note:_ If you want to build the x64 version, please add `-G "Visual Studio 17 2022" -A x64` (for VS2022) to the first cmake command.

_Note:_ If you want to download and build Nyan automatically add `-DDOWNLOAD_NYAN=YES -DFLEX_EXECUTABLE=<path to win_flex.exe>` to the first cmake command.

## Running openage (in devmode)
While this is straightforward on other platforms, there is still stuff to do to run openage on Windows:

- Install the [DejaVu Book Font](https://dejavu-fonts.github.io/Download.html).
  - Download and extract the latest `dejavu-fonts-ttf` tarball/zip file.
  - Select all `ttf\DejaVuSerif*.ttf` files, right click and click `Install for all users`.
  _Note:_ This will require administrator rights.

  - Set the `FONTCONFIG_PATH` environment variable to `<vcpkg directory>\installed\<relevant config>\tools\fontconfig\fonts\` or `<vcpkg directory>\installed\<relevant config>\etc\fonts\`.
  - Copy `fontconfig\57-dejavu-serif.conf` to `%FONTCONFIG_PATH%\conf.d`.
- [Optional] Set the `AGE2DIR` environment variable to the AoE 2 installation directory.
- Set `QML2_IMPORT_PATH` to `<vcpkg directory>\installed\<relevant config>\qml` or for prebuilt Qt `<qt directory>\<qt-version>\<compiler-version>\qml`
- openage needs these DLL files to run:
  - `openage.dll` (Usually in `<openage directory>\build\libopenage\<config built>`.)
  - `nyan.dll` (The location depends on the procedure chosen to get nyan.)
  - DLLs from vcpkg-installed dependencies. Normally, these DLLs should be copied to `<openage directory>\build\libopenage\<config built>` during the build process. If they are not, you can find them in `<vcpkg directory>\installed\<relevant config>\bin`.
    - If prebuilt QT6 was installed, the original location of QT6 DLLs is `<QT6 directory>\bin`.

Now, to run openage:

- Open a CMD window in `<openage directory>\build\` and run `python -m openage main`
- Execute`<openage directory>\build\run.exe` every time after that and enjoy!

## Packaging

- Install [NSIS](https://sourceforge.net/projects/nsis/files/latest/download).
- Depending on the way you installed Qt (vcpkg/pre-built) you need to edit the following line in `<openage-repo-dir>\buildsystem\templates\ForwardVariables.cmake.in`:

```
# Use windeploy for packaging qt-prebuilt, standard value '1' for windeploy, '0' for vcpkg
set(use_windeployqt 1)
```

Open a command prompt at `<openage directory>\build` (or use the one from the building step):

```ps
    cpack -C RelWithDebInfo
```

The installer (`openage-<version>-<arch>.exe`) will be generated in the same directory.<br>

_Note:_ Append `-V` to the `cpack` command for verbose output (it takes time to package all dependencies).

_Note:_ <arch> you can set with the environment variable `TARGET_PLATFORM` (e.g. amd64, x86).
