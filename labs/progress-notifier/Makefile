
include ../../common.mk

logger.o:
	gcc -c logger.c

base64: logger.o
	gcc -o base64 base64.c logger.o

encode: base64
	./base64 --encode input.txt

decode: base64
	./base64 --decode encoded.txt