# IFJ20
# Autors:
# Mario Harvan, xharva03
# Juraj Marticek, xmarti97
# Michal Slesar, xslesa01
# Erik Belko, xbelko02

CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra
MODULES = symtable.o   #   <----- aj tuto pridavajte moduly

CFLAGS += $(DEBUG)

all: compiler

compiler: $(MODULES) compiler.o     
	$(CC) $(CFLAGS) -o $@ $^

#moduls (ADD new modules here)

symtable.o: symtable.c
	$(CC) $(CFLAGS) -o $@ -c $^

compiler.o: compiler.c
	$(CC) $(CFLAGS) -o $@ -c $^

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