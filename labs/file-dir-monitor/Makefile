
include ../../common.mk

logger.o:
	gcc -c logger.c

monitor: logger.o
	gcc -o monitor monitor.c logger.o

run: monitor
	sudo ./monitor