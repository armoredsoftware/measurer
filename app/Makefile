
build: build-test1 build-test2 build-buffer_overflow
 
build-test1:
	gcc -g test1.c -o test1.o

build-test2:
	gcc -g test2.c -o test2.o

build-buffer_overflow:
	cd buffer_overflow ; gcc -g buffer_overflow1.c -o buffer_overflow1.o
	cd buffer_overflow ; gcc -g buffer_overflow2.c -o buffer_overflow2.o

run-test1:
	./test1.o

run-test2:
	./test2.o

clean:
	rm -rf *.o
