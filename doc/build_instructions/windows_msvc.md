# Procedure for Microsoft Windows users (Windows 10)

 Since Windows doesn't offer a native package manager, we use a mixture of manual and automated steps to get the dependencies for openage.
 *Please remember to replace the directories referenced below (written in <...>) with the appropriate values.*

## Setting up the build environment
 You will need to download and install the following manually.
 Those who already have the latest stable versions of these programs can skip this:
 - [Visual Studio 2017 Community edition](https://www.visualstudio.com/downloads/)
   - With the "Desktop development with C++" workload.
   - With the "Windows 10 SDK". Choose the latest version listed.
 - Instead of Visual Studio, you could also use the [Visual Studio Buildtools](https://download.visualstudio.microsoft.com/download/pr/10413969-2070-40ea-a0ca-30f10ec01d1d/414d8e358a8c44dc56cdac752576b402/vs_buildtools.exe), they use less space
 - [Python 3](https://www.python.org/downloads/windows/)
   - With the "pip" option enabled. We use `pip` to install other dependencies.
   - With the "Precompile standard library" option enabled.
   - With the "Download debug binaries (...)" option enabled.
   - If in doubt, run the installer again and choose "Modify".
   - You are going to need the 64-bit version of python if you are planning to build the 64-bit version of openage, and vice versa.
 - [CMake](https://cmake.org/download/)

### Python Modules
 Open a command prompt at `<Python 3 installation directory>/Scripts`

    pip install cython numpy pillow pygments pyreadline Jinja2

### vcpkg packages
 Set up [vcpkg](https://github.com/Microsoft/vcpkg#quick-start). Open a command prompt at `<vcpkg directory>`

    vcpkg install dirent eigen3 fontconfig freetype harfbuzz libepoxy libogg libpng opus opusfile qt5-base qt5-declarative qt5-quickcontrols sdl2 sdl2-image

 _Note:_ The `qt5` port in vcpkg has been split into multiple packages, build times are acceptable now.
 If you want, you can still use [the prebuilt version](https://www.qt.io/download-open-source/) instead.
 If you do so, include `-DCMAKE_PREFIX_PATH=<QT5 directory>` in the cmake configure command.

 _Note:_ If you are planning to build the 64-bit version of openage, you are going to need 64-bit libraries. Add command line option `--triplet x64-windows` to the above command or add the environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows` to build x64 libraries. [See here](https://github.com/Microsoft/vcpkg/issues/1254)

## Building openage
 Note that openage doesn't support completely out-of-source-tree builds yet.
 We will, however, use a separate `build` directory to build the binaries.

_Note:_ You will also need to set up [the dependencies for Nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md#windows).

 Open a command prompt at `<openage directory>`:

     mkdir build
     cd build
     cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg directory>/scripts/buildsystems/vcpkg.cmake ..
     cmake --build . --config RelWithDebInfo -- /nologo /m /v:m

_Note:_ If you want to build the x64 version, please add `-G "Visual Studio 15 2017 Win64"` (for VS2017) to the first cmake command.

_Note:_ If you want to download and build Nyan automatically add `-DDOWNLOAD_NYAN=YES -DFLEX_EXECUTABLE=<path to win_flex.exe>` to the first cmake command. 

## Running openage (in devmode)
 While this is straightforward on other platforms, there is still stuff to do to run openage on Windows:
  - Install the [DejaVu Book Font](https://dejavu-fonts.github.io/Download.html).
    - Download and extract the latest `dejavu-fonts-ttf` tarball/zip file.
    - Copy `ttf/DejaVuSerif*.ttf` font files to `%WINDIR%/Fonts`.
    - Set the `FONTCONFIG_PATH` environment variable to `<vcpkg directory>\installed\<relevant config>\tools\fontconfig\fonts\`.
    - Copy `fontconfig/57-dejavu-serif.conf` to `%FONTCONFIG_PATH%/conf.d`.
  - [Optional] Set the `AGE2DIR` environment variable to the AoE 2 installation directory.
  - Set `QML2_IMPORT_PATH` to `<vcpkg directory>\installed\<relevant config>\qml`
  - Append the following to the environment `PATH`:
    - `<openage directory>\build\libopenage\<config built>` (for `openage.dll`)
    - Path to `nyan.dll` (depends on the procedure chosen to get nyan)
    - `<vcpkg directory>\installed\<relevant config>\bin`
    - `<QT5 directory>\<relevant-subfolder-structure>\platforms\qwindows.dll` TODO
    - `<QT5 directory>\bin` (if prebuilt QT5 was installed)

 Now, execute `<openage directory>/run.exe` and enjoy!

## Packaging
 **Doesn't work with vcpkg installed Qt submodules.** [Open issue for `windeployqt`](https://github.com/microsoft/vcpkg/issues/1654).
 
 Open a command prompt at `<openage directory>\build` (or use the one from the building step):

_Note:_ You need [NSIS](https://nsis.sourceforge.io/Download) to be installed.

    cpack -C RelWithDebInfo

 The installer (`openage-<version>-win32.exe`) will be generated in the same directory.<br>
 _Hint_: Append `-V` to the `cpack` command for verbose output (it takes time to package all dependencies).
