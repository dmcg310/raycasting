OS := $(shell uname)

all: clean build run

build:
ifeq ($(OS),Darwin)
	gcc -std=c99 -I/opt/homebrew/include ./src/*.c -o main -L/opt/homebrew/lib -lSDL2 -lm
else
	gcc -std=c99 ./src/*.c -lSDL2 -lm -o main
endif

run:
	./main

clean:
	rm -f main

.PHONY: all build run clean
