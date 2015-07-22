build: build-driver build-app

build-driver:
	gcc -g driver.c -o driver.o -ljansson -lreadline

build-app:
	gcc -g app.c -o app.o

clean:
	rm -rf *.o
