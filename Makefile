# type 'make help' for a list/explaination of recipes.

BUILDDIR = bin

MAKEARGS += $(if $(VERBOSE),,--no-print-directory)

.PHONY: default
default: build

.PHONY: all
all: default

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
	$(BUILDDIR)/run game

.PHONY: test
test: tests checkfast

.PHONY: tests
tests: build
	$(BUILDDIR)/run test -a

.PHONY: build
build: $(BUILDDIR)
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR)

.PHONY: ninja
ninja: $(BUILDDIR)
	@ninja -C $(BUILDDIR)

.PHONY: libopenage
libopenage: $(BUILDDIR)
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR) libopenage

.PHONY: codegen
codegen: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) codegen

.PHONY: pxdgen
pxdgen: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) pxdgen

.PHONY: compilepy
compilepy: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) compilepy

.PHONY: inplacemodules
inplacemodules:
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) inplacemodules

.PHONY: cythonize
cythonize: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cythonize

.PHONY: doc
doc: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) doc

.PHONY: webdoc
webdoc: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) webdoc

.PHONY: cleanelf
cleanelf: $(BUILDDIR)
	@# removes all object files and binaries
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) clean

.PHONY: cleancodegen
cleancodegen: $(BUILDDIR)
	@# removes all sourcefiles created by codegen
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cleancodegen

.PHONY: cleanpxdgen
cleanpxdgen: $(BUILDDIR)
	@# removes all generated .pxd files
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cleanpxdgen

.PHONY: cleancython
cleancython: $(BUILDDIR)
	@# removes all .cpp files created by Cython
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cleancython

.PHONY: clean
clean: $(BUILDDIR) cleancodegen cleanpxdgen cleancython cleanelf
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
	@if test -d bin; then $(MAKE) $(MAKEARGS) -C bin clean cleancython cleanpxdgen cleancodegen || true; fi
	@echo cleaning symlinks to build directories
	rm -f bin
	@echo cleaning build directories
	rm -rf .bin
	@echo cleaning cmake-time generated code
	rm -f Doxyfile py/openage/config.py libopenage/config.h libopenage/config.cpp

.PHONY: mrproper
mrproper: cleanbuilddirs
	@echo cleaning converted assets
	rm -rf userassets

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

.PHONY: checkpy
checkpy:
	python3 -m buildsystem.codecompliance --pystyle --pylint

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
	@echo "pxdgen             -> generate .pxd files"
	@echo "cythonize          -> compile .pyx files to .cpp"
	@echo "compilepy          -> compile .py files to .pyc"
	@echo "inplacemodules     -> create in-place modules"
	@echo "codegen            -> generate cpp sources"
	@echo "doc                -> create documentation files"
	@echo "webdoc             -> create webdocumentation files"
	@echo ""
	@echo "cleanelf           -> remove C++ ELF files"
	@echo "cleancodegen       -> undo 'make codegen'"
	@echo "cleancython        -> undo 'make cythonize inplacemodules'"
	@echo "cleanpxdgen        -> undo 'make pxdgen'"
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
	@echo "checkpy            -> check python compliance"
	@echo ""
	@echo "test               -> tests + checkfast. this is what you should use for regular devbuilds"
	@echo ""
	@echo "CMake help:"
	@test -d $(BUILDDIR) && $(MAKE) -C $(BUILDDIR) help || echo "no builddir is configured"
