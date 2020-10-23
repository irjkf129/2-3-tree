all: bin/main

bin/main: obj/main.o obj/tree.o
	gcc -Wall -Werror obj/main.o obj/tree.o -o bin/main

obj/main.o: src/main.c include/tree.h
	gcc -Wall -Werror -I include -c src/main.c -o obj/main.o

obj/tree.o: src/tree.c include/tree.h
	gcc -Wall -Werror -I include -c src/tree.c -o obj/tree.o

clear:
	rm -rf obj/*.o bin/main