# type 'make help' for a list/explaination of recipes.

# original asset directory
AGE2DIR=

# this list specifies needed media files for the convert script
# TODO: don't rely on the make system, let our binary call the convert script
needed_media = graphics:*.* terrain:*.* sounds0:*.* sounds1:*.* gamedata0:*.* gamedata1:*.* gamedata2:*.* interface:*.*

binary = ./openage
runargs = --data=assets
BUILDDIR = bin

MAKEARGS += $(if $(VERBOSE),,--no-print-directory)

.PHONY: all
all: openage

$(BUILDDIR):
	@echo "call ./configure to initialize the build directory."
	@echo "also see ./configure --help, and building.md"
	@echo ""
	@false

.PHONY: openage
openage: $(BUILDDIR)
	@make $(MAKEARGS) -C $(BUILDDIR)

.PHONY: install
install: $(BUILDDIR)
	@make $(MAKEARGS) -C $(BUILDDIR) install

.PHONY: media
media: $(BUILDDIR)
	@if test ! -d "$(AGE2DIR)"; then echo "you need to specify AGE2DIR (e.g. /home/user/.wine/drive_c/age)."; false; fi
	buildsystem/runinenv PYTHONPATH=prependpath:py -- python3 -m openage.convert -v media -o "assets/converted" "$(AGE2DIR)" $(needed_media)

.PHONY: medialist
medialist:
	@echo "$(needed_media)"

.PHONY: run
run: openage
	$(binary) $(runargs)

.PHONY: runval
runval: openage
	valgrind --leak-check=full --track-origins=yes -v $(binary) $(runargs)

.PHONY: rungdb
rungdb: openage
	gdb --args $(binary) $(runargs)

.PHONY: test
test: $(binary)
	@CTEST_OUTPUT_ON_FAILURE=1 make $(MAKEARGS) -C $(BUILDDIR) test

.PHONY: codegen
codegen: $(BUILDDIR)
	@make $(MAKEARGS) -C $(BUILDDIR) codegen

.PHONY: pymodules
pymodules: $(BUILDDIR)
	@make $(MAKEARGS) -C $(BUILDDIR) pymodules

.PHONY: doc
doc: $(BUILDDIR)
	@make $(MAKEARGS) -C $(BUILDDIR) doc

.PHONY: cleanelf
cleanelf: $(BUILDDIR)
	@# removes all object files and binaries
	@make $(MAKEARGS) -C $(BUILDDIR) clean

.PHONY: cleancodegen
cleancodegen: $(BUILDDIR)
	@# removes all generated sourcefiles
	@make $(MAKEARGS) -C $(BUILDDIR) cleancodegen

.PHONY: cleanpymodules
cleanpymodules: $(BUILDDIR)
	@# removes all built python modules (+ extension modules)
	@make $(MAKEARGS) -C $(BUILDDIR) cleanpymodules
	@# removes all in-place built extension modules
	@find py -name "*.so" -type f -print -delete

.PHONY: clean
clean: $(BUILDDIR) cleancodegen cleanpymodules cleanopenage
	@# removes object files, binaries, pymodules, generated code

.PHONY: cleaninsourcebuild
cleaninsourcebuild:
	@echo "cleaning remains of in-source builds"
	rm -rf DartConfiguration.tcl codegen_depend_cache codegen_target_cache tests_cpp tests_python Doxyfile Testing py/setup.py
	@find . -not -path "./.bin/*" -type f -name CTestTestfile.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name cmake_install.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name CMakeCache.txt                   -print -delete
	@find . -not -path "./.bin/*" -type f -name Makefile -not -path "./Makefile" -print -delete
	@find . -not -path "./.bin/*" -type d -name CMakeFiles                       -print -exec rm -r {} +

.PHONY: cleanbuilddirs
cleanbuilddirs: cleaninsourcebuild
	@if test -d bin; then make $(MAKEARGS) -C bin clean || true; fi
	@echo cleaning symlinks to build directories
	rm -f openage bin
	@echo cleaning build directories
	rm -rf .bin
	@echo cleaning cmake-time generated code
	rm -f Doxyfile py/openage/config.py cpp/config.h
	@echo cleaning cmake-time generated assets
	rm -f assets/tests_py assets/tests_cpp

.PHONY: mrproper
mrproper: cleanbuilddirs
	@echo cleaning converted assets
	rm -rf userassets

.PHONY: mrproperer
mrproperer: mrproper
	@if ! test -d .git; then echo "mrproperer is only available for gitrepos."; false; fi
	@echo removing ANYTHING that is not checked into the git repo
	git clean -x -d -f

.PHONY: help
help: $(BUILDDIR)/Makefile
	@echo "openage Makefile"
	@echo ""
	@echo "wrapper that mostly forwards recipes to the cmake-generated Makefile in bin/"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "openage            -> compile main binary"
	@echo "codegen            -> generate cpp sources"
	@echo "media              -> convert media files, usage: make media AGE2DIR=~/.wine/ms-games/age2"
	@echo "medialist          -> list needed media files for current version"
	@echo "doc                -> create documentation files"
	@echo ""
	@echo "cleanelf           -> remove C++ ELF files"
	@echo "cleancodegen       -> undo 'make codegen'"
	@echo "cleanpymodules     -> undo 'make pymodules'"
	@echo "clean              -> undo 'make' (all of the above)"
	@echo "cleanbuilddirs     -> undo 'make' and './configure'"
	@echo "cleaninsourcebuild -> undo in-source build accidents"
	@echo "mrproper           -> as above, but additionally delete user assets"
	@echo "mrproperer         -> this recipe is serious business. it will leave no witnesses."
	@echo ""
	@echo "run                -> run openage"
	@echo "runval             -> run openage in valgrind, analyzing for memleaks"
	@echo "rungdb             -> run openage in gdb"
	@echo ""
	@echo ""
	@echo "CMake help:"
	@test -d $(BUILDDIR) && make -C $(BUILDDIR) help || echo "no builddir is configured"
