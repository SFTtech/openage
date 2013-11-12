#openage main makefile
#
#this actually just invokes the cmake generated makefile in bin/


AGE2DIR = /dev/null

media_path = data/age/raw/Data

needed_media = graphics:3836.slp graphics:4857.slp terrain:*.slp

#TODO: check if all needed media files have been extracare present in filesystem
#media_file_list = $(patsubst %, media_path/% ,$(subst :, .drs/, text))
#media_file_list = $(patsubst %.slp, %.slp.png, media_file_list)
#media_file_list = $(patsubst %.wav, %.opus, media_file_list)

all: openage

bin:
	mkdir -p bin/

bin/Makefile:
	$(error bin/Makefile not found, did you run ./configure?)

openage: bin/Makefile bin
	@make -C bin/


media:
	python3 convert -o . $(AGE2DIR) $(needed_media)

medialist:
	@echo "$(needed_media)"

run: openage
	@cd data && ../bin/openage

runmem: openage
	@cd data && valgrind --leak-check=full --track-origins=yes -v ../bin/openage

doc:
	@make -C bin/ doc

clean: bin/Makefile
	@make -C bin/ clean

mrproper:
	rm -rf bin/
	rm -rf data/age

help: bin/Makefile
	@echo "openage make system"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "openage   -> compile main binary"
	@echo "media     -> convert media files, usage: make AGE2DIR=~/.wine/ms-games/age2 media"
	@echo "medialist -> list needed media files for current version"
	@echo "doc       -> create documentation files"
	@echo "clean     -> clean up object files"
	@echo "mrproper  -> clean up everything: configure stuff, binary files, media files"
	@echo ""
	@echo ""
	@echo "CMake help, for make -C bin/ ...:"
	@make -C bin/ help

.PHONY: all openage doc clean mrproper media medialist help
