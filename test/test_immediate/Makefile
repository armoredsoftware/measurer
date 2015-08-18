
build: build-driver build-app

build-driver:
	gcc -DJASON_CLIENT -g driver.c ../../msrrd/gdb/ME_RLI_IR.c -o driver.o -ljansson -lreadline

build-app:
	gcc -g app.c -o app.o

clean:
	rm -rf *.o
