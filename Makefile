#openage main makefile
#
#this actually just invokes the cmake generated makefile in bin/


#the directory where the original media files are in
AGE2DIR = /dev/null

project_name = openage

#the directory where converted files will be put
DATA_DIR = ./data

#this list specifies needed media files for the convert script
#TODO: let our binary generate this list.
needed_media = graphics:*.* terrain:*.* sounds0:*.* sounds1:*.* gamedata0:*.* gamedata1:*.* gamedata2:*.* interface:*.*


binary = bin/$(project_name)

runargs = --data=$(DATA_DIR)

.PHONY: all
all: $(project_name)

bin:
	mkdir -p bin/

bin/Makefile:
	$(error bin/Makefile not found, did you run ./configure?)

.PHONY: $(project_name)
$(project_name): bin/Makefile bin
	@make -C bin/

.PHONY: install
install: bin/Makefile bin
	@make -C bin/ install

.PHONY: media
media:
	python3 convert -v media -o $(DATA_DIR)/age/ $(AGE2DIR) $(needed_media)

.PHONY: medialist
medialist:
	@echo "$(needed_media)"

.PHONY: run
run: $(project_name)
	$(binary) $(runargs)

.PHONY: runmem
runmem: $(project_name)
	valgrind --leak-check=full --track-origins=yes -v $(binary) $(runargs)

.PHONY: rungdb
rungdb: $(project_name)
	gdb --args $(binary) $(runargs)

.PHONY: doc
doc:
	@make -C bin/ doc

.PHONY: clean
clean: bin/Makefile
	@make -C bin/ clean

.PHONY: mrproper
mrproper:
	rm -rf bin/
	rm -rf data/age

.PHONY: help
help: bin/Makefile
	@echo "$(project_name) make system"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "$(project_name)   -> compile main binary"
	@echo "media     -> convert media files, usage: make media AGE2DIR=~/.wine/ms-games/age2"
	@echo "medialist -> list needed media files for current version"
	@echo "doc       -> create documentation files"
	@echo "clean     -> clean up object files"
	@echo "mrproper  -> clean up everything: configure stuff, binary files, media files"
	@echo ""
	@echo "run       -> run $(project_name)"
	@echo "runmem    -> run valgrind, analyze for memleaks."
	@echo "rungdb    -> run gdb, for debugging etc."
	@echo ""
	@echo ""
	@echo "Now follows the CMake help. (make -C bin/) :"
	@make -C bin/ help
