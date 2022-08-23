CC = clang
CFLAGS = -std=c17 -Iinclude -Llib \
	-Wall -Wextra -Wswitch -Wvla -Wimplicit-fallthrough \
	-Wno-unused-label \
	-pedantic \
	$(shell PKG_CONFIG_PATH=lib pkg-config --cflags glfw3)
CLIBS = $(shell PKG_CONFIG_PATH=lib pkgconf glfw3 --libs --static)

ifdef RELEASE
	CFLAGS += -O2 -DNDEBUG -Wpadded
else
	CFLAGS += -O0 -g
endif

main: bin/main
	bin/main

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
