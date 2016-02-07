CXX=gcc
CFLAGS=-Wall -O3 -g -std=c99
SOURCE=datablock_parser.c
HEADER=
TARGET=Prog

default: compile

compile:
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -lm

debug:
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -DEBUG -lm

clean:
	rm -f ${TARGET} *.o *.png *.dat
