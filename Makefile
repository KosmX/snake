snek: snake.o multiplatformLib.o
	gcc -O3 snake.o multiplatformLib.o -o snake

snake.o: snake.c structs.h debugmalloc.h
	gcc -O3 -c snake.c -o snake.o

multiplatformLib.o: multiplatformLib.c multiplatformLib.h
	gcc -O3 -c multiplatformLib.c -o multiplatformLib.o