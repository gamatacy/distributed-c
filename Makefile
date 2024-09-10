PCOUNT ?= 3

.PHONY: compile exact

compile:
	clang -std=c99 -Wall -pedantic *.c

exact: compile
	./a.out -p ${PCOUNT}