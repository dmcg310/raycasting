all: clean build run

build:
	gcc -std=c99 ./src/*.c -lSDL2 -lm -o main

run:
	./main

clean:
	rm -f main

.PHONY: all build run clean
