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
test: $(BUILDDIR)
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

.PHONY: cleanbin
cleanbin:
	@if test -d bin; then make -C bin clean || true; fi
	@echo build directories
	rm -rf .bin
	@echo symlinks
	rm -f openage bin
	@echo cmake-time generated files
	rm -f Doxyfile
	@echo remains from accidential in-source builds
	rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Testing

.PHONY: mrproper
mrproper: cleanbin
	@echo converted assets
	rm -rf $(DATADIR)/age

.PHONY: mrproperer
mrproperer: mrproper
	@echo remove ANYTHING that is not tracked by git
	@if ! test -d .git; then echo "mrproperer is only available for gitrepos."; false; fi
	git clean -x -d -f

.PHONY: help
help: $(BUILDDIR)/Makefile
	@echo "openage Makefile"
	@echo ""
	@echo "wrapper that mostly forwards recipes to the cmake-generated Makefile in bin/"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "openage    -> compile main binary"
	@echo "media      -> convert media files, usage: make media AGE2DIR=~/.wine/ms-games/age2"
	@echo "medialist  -> list needed media files for current version"
	@echo "doc        -> create documentation files"
	@echo ""
	@echo "clean      -> undo 'make'
	@echo "cleanbin   -> undo 'make' and './configure'
	@echo "mrproper   -> as above, but additionally delete converted assets
	@echo "mrproperer -> this recipe is serious business. it will leave no witnesses.
	@echo ""
	@echo "run        -> run openage"
	@echo "runval     -> run openage in valgrind, analyzing for memleaks"
	@echo "rungdb     -> run openage in gdb"
	@echo ""
	@echo ""
	@echo "CMake help:
	@test -d $(BUILDDIR) && make -C $(BUILDDIR) help || echo "no builddir is configured"
