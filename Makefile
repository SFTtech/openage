#openage main makefile
#
#this actually just invokes the cmake generated makefile in bin/


all: openage

bin:
	mkdir -p bin/

bin/Makefile:
	@echo "bin/Makefile not found, did you run ./configure?"

openage: bin bin/Makefile
	@make -C bin/

doc:
	@make -C bin/ doc

clean:
	@make -C bin/ clean

mrproper: clean
	@make -C bin/ clean
	rm -rf bin/

help:
	@make -C bin/ help

.PHONY: all openage doc clean mrproper help
