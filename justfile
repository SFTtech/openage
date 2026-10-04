# openage build wrapper; forwards recipes to the build dir (ninja by default)
# run `just --list` to list the recipes; see ./configure for build dir setup

builddir := "bin"

# forward to ninja; make is only relevant for --makefile-generator build dirs
buildcmd := `test -f bin/build.ninja && echo "ninja -C bin" || echo "make --no-print-directory -C bin"`

# build the entire project
build:
	{{buildcmd}}

# build libopenage only
libopenage:
	{{buildcmd}} libopenage

# install openage
install:
	{{buildcmd}} install

# generate the C++ sources from the codegen definitions
codegen:
	{{buildcmd}} cppgen

# generate the .pxd files from the C++ headers
pxdgen:
	{{buildcmd}} pxdgen

# compile the .pyx files to .cpp
cythonize:
	{{buildcmd}} cythonize

# create the in-place python modules
inplacemodules:
	{{buildcmd}} inplacemodules

# compile the .py files to .pyc
compilepy:
	{{buildcmd}} compilepy

# create the documentation
doc:
	{{buildcmd}} doc

# run the game
run: build
	cd {{builddir}} && ./run main

# run the tests (C++ and python)
tests: build
	cd {{builddir}} && ./run test -a

# tests + checkfast; the recipe for regular devbuilds
test: tests checkfast

# remove object files and binaries
cleanelf:
	{{buildcmd}} clean

# remove the sources created by codegen
cleancodegen:
	{{buildcmd}} cleancodegen

# remove the generated .pxd files
cleanpxdgen:
	{{buildcmd}} cleanpxdgen

# remove the .cpp files created by cython
cleancython:
	{{buildcmd}} cleancython

# remove object files, binaries, python modules and generated code
clean: cleancodegen cleanpxdgen cleancython cleanelf

# remove remains of in-source builds
cleaninsourcebuild:
	@echo "cleaning remains of in-source builds"
	rm -rf DartConfiguration.tcl codegen_depend_cache codegen_target_cache Doxyfile Testing
	@find . -not -path "./.bin/*" -type f -name CTestTestfile.cmake -print -delete
	@find . -not -path "./.bin/*" -type f -name cmake_install.cmake -print -delete
	@find . -not -path "./.bin/*" -type f -name CMakeCache.txt -print -delete
	@find . -not -path "./.bin/*" -type f -name Makefile -print -delete
	@find . -not -path "./.bin/*" -type d -name CMakeFiles -print -exec rm -r {} +
	@find . -not -path "./.bin/*" -type d -name __pycache__ -print -exec rm -r {} +

# remove the build directories and cmake-time generated code
cleanbuilddirs: cleaninsourcebuild
	@if test -d {{builddir}}; then {{buildcmd}} clean cleancython cleanpxdgen cleancodegen || true; fi
	@echo cleaning symlinks to build directories
	rm -f {{builddir}}
	@echo cleaning build directories
	rm -rf .bin
	@echo cleaning cmake-time generated code
	rm -f Doxyfile openage/config.py libopenage/config.h libopenage/config.cpp

# additionally remove the converted assets
mrproper: cleanbuilddirs
	@echo cleaning converted assets
	rm -rf userassets

# additionally remove anything not checked into the git repo
mrproperer: mrproper
	@if ! test -d .git; then echo "mrproperer is only available for gitrepos."; false; fi
	@echo removing ANYTHING that is not checked into the git repo
	@echo ENTER to confirm
	@read val && git clean -x -d -f

# fast code compliance checks
checkfast:
	uv run python3 -m buildsystem.codecompliance --fast

# code compliance checks for merging to master
checkmerge:
	uv run python3 -m buildsystem.codecompliance --merge

# full code compliance checks for files changed since origin/master
checkchanged:
	uv run python3 -m buildsystem.codecompliance --merge --only-changed-files=origin/master

# full code compliance checks for uncommitted files
checkuncommited:
	uv run python3 -m buildsystem.codecompliance --merge --only-changed-files=HEAD

# python compliance checks
checkpy:
	uv run python3 -m buildsystem.codecompliance --ruff --ty

# full code compliance check
checkall:
	uv run python3 -m buildsystem.codecompliance --all
