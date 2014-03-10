#openage main makefile
#
#this actually just invokes the cmake generated makefile in bin/


#the directory where the original media files are in
AGE2DIR = /dev/null

needed_media = graphics:3836.slp graphics:4857.slp terrain:*.slp

.PHONY: all
all: openage

bin:
	mkdir -p bin/

bin/Makefile:
	$(error bin/Makefile not found, did you run ./configure?)

.PHONY: openage
openage: bin/Makefile bin
	@make -C bin/

.PHONY: install
install: bin/Makefile bin
	@make -C bin/ install

.PHONY: media
media:
	python3 convert -v media -o data/age/ $(AGE2DIR) $(needed_media)

.PHONY: medialist
medialist:
	@echo "$(needed_media)"

.PHONY: run
run: openage
	@cd data && ../bin/openage

.PHONY: runmem
runmem: openage
	@cd data && valgrind --leak-check=full --track-origins=yes -v ../bin/openage

.PHONY: rungdb
rungdb: openage
	@cd data && gdb ../bin/openage

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
	@echo "openage make system"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "openage   -> compile main binary"
	@echo "media     -> convert media files, usage: make media AGE2DIR=~/.wine/ms-games/age2"
	@echo "medialist -> list needed media files for current version"
	@echo "doc       -> create documentation files"
	@echo "clean     -> clean up object files"
	@echo "mrproper  -> clean up everything: configure stuff, binary files, media files"
	@echo ""
	@echo "run       -> run openage"
	@echo "runmem    -> run valgrind, analyze for memleaks."
	@echo "rungdb    -> run gdb, for debugging etc."
	@echo ""
	@echo ""
	@echo "Now follows the CMake help. (make -C bin/) :"
	@make -C bin/ help
