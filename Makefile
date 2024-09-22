PCOUNT ?= 3
LD_LIBRARY_PATH ?="$LD_LIBRARY_PATH:$(shell pwd)"

LD_PRELOAD ?= $(shell pwd)/lib64/libruntime.so

.PHONY: compile exact

compile:
	clang -g -std=c99 -Wall -pedantic *.c -L./lib64 -lruntime

exact: compile
	LD_PRELOAD=$(LD_PRELOAD) ./a.out -p ${PCOUNT} 10 20 30

debug: compile
	gdb ./a.out -p ${PCOUNT} 10 20 30