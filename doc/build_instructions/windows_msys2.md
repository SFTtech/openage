# Procedure for building openage on Windows 10 with MSYS2

__NOTE:__ We also have an installer for Win10 (x64), if you just want to play around with *openage* you can find it [here](https://github.com/SFTtech/openage/releases).

 Since Windows doesn't offer a native package manager, we use MSYS2 ecosystem and pull in dependencies for openage as [packages via Pacman](https://www.msys2.org/wiki/Using-packages/).

 *Please remember to replace the directories referenced below (written in <...>) with the appropriate values.*

## Dependencies
### MSYS2
  - Download and follow the install instructions for [MSYS2](https://www.msys2.org/)
  - Start a MSYS2-shell from `<MSYS2-directory>/msys2_shell.cmd`
  - Install the dependencies (make sure you have an up-to-date package database):

For clang: `pacman -Syu --needed mingw-w64-x86_64-clang mingw-w64-x86_64-libc++ mingw-w64-x86_64-libc++abi mingw-w64-x86_64-lld mingw-w64-x86_64-compiler-rt`

`pacman -Syu --needed git mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-eigen3 mingw-w64-x86_64-cython mingw-w64-x86_64-libepoxy mingw-w64-x86_64-libogg mingw-w64-x86_64-libpng mingw-w64-x86_64-ttf-dejavu mingw-w64-x86_64-freetype mingw-w64-x86_64-fontconfig mingw-w64-x86_64-harfbuzz mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2 mingw-w64-x86_64-opusfile mingw-w64-x86_64-opus mingw-w64-x86_64-python mingw-w64-x86_64-python-pip mingw-w64-x86_64-python-pillow mingw-w64-x86_64-python-numpy mingw-w64-x86_64-cython mingw-w64-x86_64-python-pygments mingw-w64-x86_64-python-jinja mingw-w64-x86_64-python-pyreadline mingw-w64-x86_64-qt5`

__**Note:**__ You will need to set up [the dependencies for Nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md#windows) and build it.
We set `nyan_DIR` to the build directory containing `nyan.dll` later in the building step.


#### Add MingW64 Binary Directory To `$PATH`

Add the `bin`-directory to your `PATH` (in MSYS2 bash):
```
echo "export PATH=/mingw64/bin:mingw64\x86_64-w64-mingw32\bin:$PATH" >> ~/.bashrc
source ~/.bashrc
```

## Building openage
 Note that openage doesn't support completely out-of-source-tree builds yet.
 We will, however, use a separate `build` directory to build the binaries.

__**IMPORTANT:**__ [`MinGW Makefiles`](https://cmake.org/cmake/help/v3.12/generator/MinGW%20Makefiles.html#generator:MinGW%20Makefiles) flag for the generator, means that CMake generates makefiles for use with `mingw32-make`
under a Windows command prompt (CMD). So you need to add the path to the `<msys2-install-dir>/mingw64/bin` as well to your [Windows `PATH` environment variable](https://lmgtfy.com/?q=windows+add+folder+to+path+environment&s=d).

```
mkdir build
cd build
-Wno-dev
-DCMAKE_MAKE_PROGRAM:FILEPATH="C:\msys64\mingw64\bin\mingw32-make.exe"
-Dnyan_DIR="../../nyan/build"
-DCMAKE_LINKER:FILEPATH="C:/msys64/mingw64/bin/lld-link.exe"
-DCMAKE_OBJCOPY:FILEPATH="C:/msys64/mingw64/bin/llvm-objcopy.exe"
-DCMAKE_STRIP:FILEPATH="C:/msys64/mingw64/bin/llvm-strip.exe"
-DCMAKE_RC_COMPILER:FILEPATH="C:/msys64/mingw64/bin/windres.exe"
-DCMAKE_ADDR2LINE:FILEPATH="C:/msys64/mingw64/bin/llvm-addr2line.exe"
-DCMAKE_OBJDUMP:FILEPATH="C:/msys64/mingw64/bin/llvm-objdump.exe"
-DCMAKE_AR:FILEPATH="C:/msys64/mingw64/bin/llvm-ar.exe"
-DCMAKE_RANLIB:FILEPATH="C:/msys64/mingw64/bin/llvm-ranlib.exe"
-DCMAKE_NM:FILEPATH="C:/msys64/mingw64/bin/llvm-nm.exe"
-DCMAKE_DLLTOOL:FILEPATH="C:/msys64/mingw64/bin/llvm-dlltool.exe"
-DCMAKE_READELF:FILEPATH="C:/msys64/mingw64/bin/llvm-readelf.exe"
-G
"CodeBlocks - MinGW Makefiles"
..
```

Navigate to the `build`-directory within your systems file explorer and put `cmd` into the adress bar. This should start up a cmd shell inside that folder.
Now start the build process in the cmd shell with this command:

`mingw32-make -j2 VERBOSE=1`


__**Note (manually prebuilt nyan):**__ If you built nyan separately and it wasn't automatically registered with CMake
you will need to set `-Dnyan_DIR=<nyan-Directory>` to the `build`-Directory [containing `nyan.dll`]

__**Note (automatically build nyan):**__ If you want to download and build Nyan automatically install these additional dependencies:

`pacman -Syu mingw-w64-x86_64-dlfcn flex`
and add `-DDOWNLOAD_NYAN=YES -DFLEX_EXECUTABLE=/usr/bin/flex.exe` to the first cmake command

## Running openage
 While this is straightforward on other platforms, there is still stuff to do to run openage on Windows:

   - Install the [DejaVu Book Font](https://dejavu-fonts.github.io/Download.html).
    - Download and extract the latest `dejavu-fonts-ttf` tarball/zip file.
    - Select all `ttf\DejaVuSerif*.ttf` files, right click and click `Install for all users`.

    _Note:_ This will require administrator rights.

  - [Optional] Set the `AGE2DIR` environment variable to the AoE 2 installation directory.

  - Set `QML2_IMPORT_PATH` to `<msys2-path>\mingw64\share\qt5\qml`

  - Append the following to the environment `PATH`:
    - `<openage build directory>\libopenage\` (for `libopenage.dll`)
    - `<nyan build directory>\nyan\` (for `libnyan.dll`) (depends on the procedure chosen to get nyan)
    - `<msys2-path>/mingw64/bin`

    __Note:__ The paths above should be added to the global system variables `PATH` and not to the user specific variables.


## Packaging
> Work in Progress
