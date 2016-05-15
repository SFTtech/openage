# type 'make help' for a list/explaination of recipes.

BUILDDIR = bin

PYLIBRARY_DIR = $(BUILDDIR)/pylibrary/lib

MAKEARGS += $(if $(VERBOSE),,--no-print-directory)

.PHONY: default
default: build

$(BUILDDIR):
	@echo "call ./configure to initialize the build directory."
	@echo "also see ./configure --help, and doc/building.md"
	@echo ""
	@false

.PHONY: install
install: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) install

.PHONY: run
run: build
	$(PYLIBRARY_DIR)/run game

.PHONY: test
test: tests checkfast

.PHONY: tests
tests: build
	$(PYLIBRARY_DIR)/run test -a

.PHONY: build
build: $(BUILDDIR)
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR)

.PHONY: libopenage
libopenage: $(BUILDDIR)
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR) libopenage

.PHONY: codegen
codegen: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) codegen

.PHONY: doc
doc: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) doc

.PHONY: cleanelf
cleanelf: $(BUILDDIR)
	@# removes all object files and binaries
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) clean

.PHONY: cleancodegen
cleancodegen: $(BUILDDIR)
	@# removes all sourcefiles created by codegen
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cleancodegen

.PHONY: clean
clean: $(BUILDDIR) cleancodegen cleanelf
	@# removes object files, binaries, py modules, generated code

.PHONY: cleaninsourcebuild
cleaninsourcebuild:
	@echo "cleaning remains of in-source builds"
	rm -rf DartConfiguration.tcl codegen_depend_cache codegen_target_cache Doxyfile Testing
	@find . -not -path "./.bin/*" -type f -name CTestTestfile.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name cmake_install.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name CMakeCache.txt                   -print -delete
	@find . -not -path "./.bin/*" -type f -name Makefile -not -path "./Makefile" -print -delete
	@find . -not -path "./.bin/*" -type d -name CMakeFiles                       -print -exec rm -r {} +

.PHONY: cleanbuilddirs
cleanbuilddirs: cleaninsourcebuild
	@if test -d bin; then $(MAKE) $(MAKEARGS) -C bin clean cleanpxdgen cleancodegen || true; fi
	@echo cleaning symlinks to build directories
	rm -f bin
	@echo cleaning build directories
	rm -rf .bin

.PHONY: mrproper
mrproper: cleanbuilddirs
	@echo cleaning converted assets
	rm -rf assets/converted

.PHONY: mrproperer
mrproperer: mrproper
	@if ! test -d .git; then echo "mrproperer is only available for gitrepos."; false; fi
	@echo removing ANYTHING that is not checked into the git repo
	@echo ENTER to confirm
	@read val
	git clean -x -d -f

.PHONY: checkfast
checkfast:
	python3 -m buildsystem.codecompliance --fast

.PHONY: checkall
checkall:
	python3 -m buildsystem.codecompliance --all

.PHONY: checkchanged
checkchanged:
	python3 -m buildsystem.codecompliance --all --only-changed-files=origin/master

.PHONY: checkuncommited
checkuncommited:
	python3 -m buildsystem.codecompliance --all --only-changed-files=HEAD

.PHONY: help
help: $(BUILDDIR)/Makefile
	@echo "openage Makefile"
	@echo ""
	@echo "wrapper that mostly forwards recipes to the cmake-generated Makefile in bin/"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "build              -> build entire project"
	@echo "libopenage         -> build libopenage"
	@echo "codegen            -> generate cpp sources"
	@echo "doc                -> create documentation files"
	@echo ""
	@echo "cleanelf           -> remove C++ ELF files"
	@echo "cleancodegen       -> undo 'make codegen'"
	@echo "clean              -> undo 'make' (all of the above)"
	@echo "cleanbuilddirs     -> undo 'make' and './configure'"
	@echo "cleaninsourcebuild -> undo in-source build accidents"
	@echo "mrproper           -> as above, but additionally delete user assets"
	@echo "mrproperer         -> leaves nothing but ashes"
	@echo ""
	@echo "run                -> run openage"
	@echo "tests              -> run the tests (py + cpp)"
	@echo ""
	@echo "checkall           -> full code compliance check"
	@echo "checkfast          -> fast checks only"
	@echo "checkchanged       -> full check for all files changed since origin/master"
	@echo "checkuncommited    -> full check for all currently uncommited files"
	@echo ""
	@echo "test               -> tests + checkfast. this is what you should use for regular devbuilds"
	@echo ""
	@echo "CMake help:"
	@test -d $(BUILDDIR) && $(MAKE) -C $(BUILDDIR) help || echo "no builddir is configured"
