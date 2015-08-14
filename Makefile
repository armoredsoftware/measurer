current_dir = $(shell pwd)

all: build-measurer build-test build-client

configure-measurer:
	cd mssrd; make clean; ./configure
	cp mssrd/patch/Makefile mssrd/gdb/

syscall-measurer:
	sudo ln -s `pwd`/mssrd/gdb /usr/local/share/gdb

build-measurer:
	cd mssrd; make

build-client:
	cd client; make

build-test:
	cd test; make

run-measurer:
	./mssrd/gdb/mssrd --port=${PORT}

run-client:
	./mssr/mssr ${PORT}

make run-test:
	cd test; make run-all

clean: clean-measurer clean-client clean-test

clean-measurer:
	cd msrrd; make clean

clean-client:
	cd msrr; make clean

clean-test:
	cd test; make clean
