CXX=gcc
CFLAGS=-Wall -O3 -g -std=c99
SOURCE=parser.c
HEADER=
TARGET=Parser

default: compile

compile:
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -lm

#for GDB
debug: new.c
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -DEBUG -lm

clean:
	rm -f ${TARGET} *.o
