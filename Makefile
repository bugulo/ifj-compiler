# IFJ20
# Autors:
# Mario Harvan, xharva03
# Juraj Marticek, xmarti97
# Michal Slesar, xslesa01
# Erik Belko, xbelko02

CC = gcc
CFLAGS = -g -std=c99 -pedantic -Wall -Wextra -Wno-discarded-qualifiers
MODULES = symtable.o string.o scanner.o error.o stack.o parser.o vector.o expression.o semantic_analysis.o file.o code_gen.o

CFLAGS += $(DEBUG)

all: compiler

compiler: $(MODULES) compiler.o     
	$(CC) $(CFLAGS) -o $@ $(MODULES) compiler.o

#modules (ADD new modules here)

symtable.o: symtable.c
	$(CC) $(CFLAGS) -o $@ -c symtable.c

compiler.o: compiler.c
	$(CC) $(CFLAGS) -o $@ -c compiler.c

string.o: string.c
	$(CC) $(CFLAGS) -o $@ -c string.c

scanner.o: scanner.c
	$(CC) $(CFLAGS) -o $@ -c scanner.c

parser.o: parser.c
	$(CC) $(CFLAGS) -o $@ -c parser.c
	
error.o: error.c
	$(CC) $(CFLAGS) -o $@ -c error.c

stack.o: stack.c
	$(CC) $(CFLAGS) -o $@ -c stack.c

vector.o: vector.c
	$(CC) $(CFLAGS) -o $@ -c vector.c

expression.o: expression.c
	$(CC) $(CFLAGS) -o $@ -c expression.c

semantic_analysis.o: semantic_analysis.c
	$(CC) $(CFLAGS) -o $@ -c semantic_analysis.c

file.o: file.c
	$(CC) $(CFLAGS) -o $@ -c file.c

code_gen.o: code_gen.c
	$(CC) $(CFLAGS) -o $@ -c code_gen.c

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
	zip xharva03.zip *.c *.h Makefile rozdeleni.txt