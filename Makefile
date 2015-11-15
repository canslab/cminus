
CC = gcc

# debug option
CFLAGS = -g	-w

#OBJS = main.o util.o scan.o #parse.o symtab.o analyze.o code.o cgen.o
OBJSPARSE = main.o util.o parse.o lex.yy.o symtab.o analyze.o 

#cminus: $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o cminus

main.o: main.c globals.h util.h scan.h #parse.h analyze.h cgen.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

parse.o: parse.c parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.c

symtab.o: symtab.c symtab.h globals.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c globals.h symtab.h analyze.h globals.h
	$(CC) $(CFLAGS) -c analyze.c

#code.o: code.c code.h globals.h
#	$(CC) $(CFLAGS) -c code.c

#cgen.o: cgen.c globals.h symtab.h code.h cgen.h
#	$(CC) $(CFLAGS) -c cgen.c

#by flex, yacc
cminus_parse: main.o util.o parse.o lex.yy.o symtab.o analyze.o
	$(CC) $(CFLAGS) main.o util.o parse.o lex.yy.o symtab.o analyze.o -o cminus_parse -lfl

lex.yy.o: cminus.l scan.h util.h globals.h
	flex -o lex.yy.c cminus.l
	$(CC) $(CFLAGS) -c lex.yy.c -lfl

clean:
	-rm cminus_parse
	# -rm tm
	-rm $(OBJSPARSE)

#tm: tm.c
#	$(CC) $(CFLAGS) tm.c -o tm

all: cminus_parse

