CC = clang
CFLAGS = -std=c2x -Iinclude -Llib \
	-Wall -Wextra -Wswitch -Wvla -Wimplicit-fallthrough \
	-Wno-unused-label -Wno-unused-parameter \
	-pedantic \
	$(shell PKG_CONFIG_PATH=lib pkg-config --cflags glfw3)
CLIBS = $(shell PKG_CONFIG_PATH=lib pkgconf glfw3 --libs --static)

ifdef RELEASE
	CFLAGS += -O2 -DNDEBUG -Wpadded
else
	CFLAGS += -O0 -g
endif

.PHONY: main
main: bin/main
	bin/main

bin/main: $(wildcard src/*.c) $(wildcard src/*.h)
	$(CC) $(CFLAGS) src/main.c src/gl3w.c -o bin/main $(CLIBS)

clean:
	rm bin/*

# todo:
#		- proper dependency injection
# 		- https://github.com/vpoulailleau/basicmakefile/blob/master/Makefile
# 		- http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#tldr
#		- dettect allocations
#		- measure performance
bin/%: src/%.c $(wildcard src/*.h)
	$(CC) $(CFLAGS) src/$*.c -o bin/$* $(CLIBS)
