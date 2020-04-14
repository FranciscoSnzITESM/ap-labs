
include ../../common.mk

logger.o:
	gcc -c logger.c

test: logger.o
	gcc -o test testLogger.c logger.o -lm

runTest: test
	./test

clean:
	rm logger.o test