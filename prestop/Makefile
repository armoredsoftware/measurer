build: build-default build-time build-prefork

build-default:
	gcc -g -c -Wall -Werror -fpic prestop.c -D_GNU_SOURCE
	gcc -g -shared -o prestop.so prestop.o -ldl

build-time:
	gcc -g -c -Wall -Werror -fpic time.c -D_GNU_SOURCE
	gcc -g -shared -o time.so time.o -ldl

build-prefork:
	gcc -g -c -Wall -Werror -fpic prefork.c -D_GNU_SOURCE
	gcc -g -shared -o prefork.so prefork.o -ldl
