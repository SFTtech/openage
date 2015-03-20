# type 'make help' for a list/explaination of recipes.

# original asset directory
BUILDDIR = bin

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
	python3 -m openage.game

.PHONY: tests
tests: build
	python3 -m openage test -a

.PHONY: test
test: tests checkfast

.PHONY: build
build: $(BUILDDIR)
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR)

.PHONY: openage
openage: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) openage

.PHONY: codegen
codegen: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) codegen

.PHONY: pxdgen
pxdgen: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) pxdgen

.PHONY: python
python: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) python

.PHONY: doc
doc: $(BUILDDIR)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) doc

.PHONY: cleanelf
cleanelf: $(BUILDDIR)
	@# removes all object files and binaries
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) clean

.PHONY: cleancodegen
cleancodegen: $(BUILDDIR)
	@# removes all generated sourcefiles
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cleancodegen

.PHONY: cleanpxdgen
cleanpxdgen: $(BUILDDIR)
	@# removes all generated .pxd files
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cleanpxdgen

.PHONY: cleanpython
cleanpython: $(BUILDDIR)
	@# removes all built python modules (+ extension modules)
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) cleanpython
	@# removes all in-place built extension modules
	@find openage -name "*.so" -type f -print -delete

.PHONY: clean
clean: $(BUILDDIR) cleancodegen cleanpython cleanpxdgen cleanelf
	@# removes object files, binaries, py modules, generated code

.PHONY: cleaninsourcebuild
cleaninsourcebuild:
	@echo "cleaning remains of in-source builds"
	rm -rf DartConfiguration.tcl codegen_depend_cache codegen_target_cache Doxyfile Testing py/setup.py
	@find . -not -path "./.bin/*" -type f -name CTestTestfile.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name cmake_install.cmake              -print -delete
	@find . -not -path "./.bin/*" -type f -name CMakeCache.txt                   -print -delete
	@find . -not -path "./.bin/*" -type f -name Makefile -not -path "./Makefile" -print -delete
	@find . -not -path "./.bin/*" -type d -name CMakeFiles                       -print -exec rm -r {} +

.PHONY: cleanbuilddirs
cleanbuilddirs: cleaninsourcebuild
	@if test -d bin; then $(MAKE) $(MAKEARGS) -C bin clean || true; fi
	@echo cleaning symlinks to build directories
	rm -f bin
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
	@echo "openage            -> build libopenage"
	@echo "pxdgen             -> generate .pxd files"
	@echo "python             -> compile python modules"
	@echo "codegen            -> generate cpp sources"
	@echo "doc                -> create documentation files"
	@echo ""
	@echo "cleanelf           -> remove C++ ELF files"
	@echo "cleancodegen       -> undo '$(MAKE) codegen'"
	@echo "cleanpython        -> undo '$(MAKE) python'"
	@echo "cleanpxdgen        -> undo '$(MAKE) pxdgen'"
	@echo "clean              -> undo '$(MAKE)' (all of the above)"
	@echo "cleanbuilddirs     -> undo '$(MAKE)' and './configure'"
	@echo "cleaninsourcebuild -> undo in-source build accidents"
	@echo "mrproper           -> as above, but additionally delete user assets"
	@echo "mrproperer         -> this recipe is serious business. it will leave no witnesses."
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
