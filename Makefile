current_dir = $(shell pwd)

all: build-measurer build-test build-client

configure-measurer:
	cd gdb-7.9; make clean; ./configure
	cp gdb-7.9/patch/Makefile gdb-7.9/gdb/

syscall-measurer:
	sudo ln -s `pwd`/gdb-7.9/gdb /usr/local/share/gdb

build-measurer:
	cd gdb-7.9; make

build-client:
	cd client; make

build-test:
	cd test; make

run-measurer:
	./gdb-7.9/gdb/gdb --port=${PORT}

run-client:
	./client/client.o ${PORT}

make run-test:
	cd test; make run-all

clean: clean-measurer clean-client clean-test

clean-measurer:
	cd gdb-7.9; make clean

clean-client:
	cd client; make clean

clean-test:
	cd test; make clean
