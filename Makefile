# IFJ20
# Autors:
# Mario Harvan, xharva03
# Juraj Marticek, xmarti97
# Michal Slesar, xslesa01
# Erik Belko, xbelko02

CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra
MODULES = symtable.o string.o scanner.o error.o stack.o  #   <----- aj tuto pridavajte moduly

CFLAGS += $(DEBUG)

all: compiler

compiler: $(MODULES) compiler.o     
	$(CC) $(CFLAGS) -o $@ $(MODULES) compiler.o

#moduls (ADD new modules here)

symtable.o: symtable.c
	$(CC) $(CFLAGS) -o $@ -c symtable.c

compiler.o: compiler.c
	$(CC) $(CFLAGS) -o $@ -c compiler.c

string.o: string.c
	$(CC) $(CFLAGS) -o $@ -c string.c

scanner.o: scanner.c
	$(CC) $(CFLAGS) -o $@ -c scanner.c
	
error.o: error.c
	$(CC) $(CFLAGS) -o $@ -c error.c

stack.o: stack.c
	$(CC) $(CFLAGS) -o $@ -c stack.c

###############################

#TESTING

test: test.o $(MODULES)
	$(CC) $(CFLAGS) -o $@ $^

test.o: test.c
	$(CC) $(CFLAGS) -o $@ -c $^

###############################

clean:
	rm -f *.o compiler test xharva03.zip

#PACK

pack: compiler
	zip xharva03.zip *.c *.h Makefile