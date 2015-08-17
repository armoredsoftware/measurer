
build: build-client

build-client:
	gcc -DJASON_CLIENT msrr.c ../msrrd/gdb/ME_common.c ../msrrd/gdb/ME_RLI_IR.c -o msrr -lreadline -lcurses -ljansson

run-client:
	./msrr

clean:
	rm -rf *.o
