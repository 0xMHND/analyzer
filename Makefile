CXX=gcc
CFLAGS=-Wall -O3 -g -std=c99
SOURCE=main.c
HEADER=main.h
TARGET=Prog

default: compile

diff:
	${CXX} ${CFLAGS} differential.c -o differential -lm
	

compile:
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -lm
main2:
	${CXX} ${CFLAGS} main2.c ${HEADER} -o ${TARGET} -lm

debug:
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -DEBUG -lm

rmlogs:
	rm -rf ./logs/

clean: rmlogs
	rm -f ${TARGET} differential *.o *.png *.dat

du:
	du -a ~ | sort -n -r | head -n 10

dblog:
	python ~/analyzer/block-args.py -p ~/xstg/tg/tg/install/bin/ -b ~/xstg/apps/apps/examples/fib/build/tg/fib.dbg -l ~/parsed/fib.log.rck000.cub00.skt0.cls000.blk00.XE.00 &> dblog.out
dblog-xe1:
	python ~/analyzer/block-args.py -p ~/xstg/tg/tg/install/bin/ -b ~/xstg/apps/apps/examples/fib/build/tg/fib.dbg -l ~/parsed/fib.log.rck000.cub00.skt0.cls000.blk00.XE.00 &> dblog-xe1.out
ocrparse:
	python ~/analyzer/xe-analysis.py -p ~/xstg/tg/tg/install/bin/ -b ~/xstg/apps/apps/examples/fib/build/tg/fib.dbg -l ~/parsed/fib.log.rck000.cub00.skt0.cls000.blk00.XE.00 &> ~/parsed2/fib.XE.00.OCR-parsed.out
