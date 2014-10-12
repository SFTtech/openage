# type 'make help' for a list/explaination of recipes.

# openage asset directory
DATADIR=data
# original asset directory
AGE2DIR=

# this list specifies needed media files for the convert script
# TODO: don't rely on the make system, let our binary call the convert script
needed_media = graphics:*.* terrain:*.* sounds0:*.* sounds1:*.* gamedata0:*.* gamedata1:*.* gamedata2:*.* interface:*.*

binary = ./openage
runargs = --data=$(DATADIR)
BUILDDIR = bin

.PHONY: all
all: openage

$(BUILDDIR):
	@echo "call ./configure to initialize the build directory."
	@echo "also see ./configure --help, and building.md"
	@echo ""
	@false

.PHONY: openage
openage: $(BUILDDIR)
	@make -C $(BUILDDIR)

.PHONY: install
install: $(BUILDDIR)
	@make -C $(BUILDDIR) install

.PHONY: media
media: $(BUILDDIR)
	@if test ! -d "$(AGE2DIR)"; then echo "you need to specify AGE2DIR (e.g. ~/.wine/drive_c/age)."; false; fi
	buildsystem/runinenv PYTHONPATH=prependpath:py python3 -m openage.convert -v media -o "$(DATADIR)/age/" "$(AGE2DIR)" $(needed_media)

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
	@make -C $(BUILDDIR) test

.PHONY: codegen
codegen: $(BUILDDIR)
	@make -C $(BUILDDIR) codegen

.PHONY: doc
doc: $(BUILDDIR)
	@make -C $(BUILDDIR) doc

.PHONY: cleancodegen
cleancodegen: $(BUILDDIR)
	# removes all generated sourcefiles
	@make -C $(BUILDDIR) cleancodegen

.PHONY: clean
clean: $(BUILDDIR)
	# removes all objects and binaries
	@make -C $(BUILDDIR) clean

.PHONY: cleaninsourcebuild
cleaninsourcebuild:
	@echo "cleaning remains of in-source builds"
	rm -rf DartConfiguration.tcl codegen_depend_cache codegen_target_cache tests_cpp tests_python Doxyfile Testing py/setup.py
	@find . -not -path "./.bin/*" -type f -name CTestTestfile.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name cmake_install.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name CMakeCache.txt                   -print -delete
	@find . -not -path "./.bin/*" -type f -name Makefile -not -path "./Makefile" -print -delete
	@find . -not -path "./.bin/*" -type d -name CMakeFiles                       -print -exec rm -r {} +

.PHONY: cleanbin
cleanbin: cleaninsourcebuild
	@if test -d bin; then make -C bin clean || true; fi
	@echo cleaning symlinks to build directories
	rm -f openage bin
	@echo cleaning build directories
	rm -rf .bin
	@echo cleaning cmake-time generated files
	rm -f Doxyfile

.PHONY: mrproper
mrproper: cleanbin
	@echo cleaning converted assets
	rm -rf $(DATADIR)/age

.PHONY: mrproperer
mrproperer: mrproper
	@if ! test -d .git; then echo "mrproperer is only available for gitrepos."; false; fi
	@echo removeing ANYTHING that is not checked into the git repo
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
	@echo "cleancodegen       -> undo 'make codegen'"
	@echo "clean              -> undo 'make' (includes the above)"
	@echo "cleaninsourcebuild -> "
	@echo "cleanbin           -> undo 'make' and './configure'"
	@echo "mrproper           -> as above, but additionally delete converted assets"
	@echo "mrproperer         -> this recipe is serious business. it will leave no witnesses."
	@echo ""
	@echo "run                -> run openage"
	@echo "runval             -> run openage in valgrind, analyzing for memleaks"
	@echo "rungdb             -> run openage in gdb"
	@echo ""
	@echo ""
	@echo "CMake help:"
	@test -d $(BUILDDIR) && make -C $(BUILDDIR) help || echo "no builddir is configured"
