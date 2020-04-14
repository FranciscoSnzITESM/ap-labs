#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include "logger.h"

char ch[2];

int writeFloat(double f){
	char *str;
	int decimal, sign;
	str = fcvt(f, 6, &decimal, &sign);
	if(sign){
		ch[0] = '-';
		fwrite(ch, sizeof(char), 1, stdout);
	}
	int i=0;
	while((ch[0] = str[i++])!= '\0'){
		if(i == decimal + 1){
			char c = ch[0];
			ch[0] = '.';
			fwrite(ch, sizeof(char), 1, stdout);
			ch[0] = c;
		}
		fwrite(ch, sizeof(char), 1, stdout);
	}										//.
	return (floor(log10(abs((int) f))) + 1) + 1 + sign + 6; // 6: Default Presition
}
int writeInt(unsigned int d){
	if(d<10){
		ch[0] = d + '0';
		fwrite(ch, sizeof(char), 1, stdout);
		return 1;
	}
	int count = writeInt(d/10);
	ch[0] = d % 10 + '0';
	fwrite(ch, sizeof(char), 1, stdout);
	return count + 1;
}
char writeBitChar(int c, int upperCase){
	if(c<=9){
		return c + '0';
	} else if(upperCase){
		return c - 10 + 'A';
	} else{
		return c - 10 + 'a';
	}
}
// digitLen : number of bits per digit
int writeHex(long p, int digitLen, int upperCase){
	if(p<pow(2, digitLen)){
		ch[0] = writeBitChar(p, upperCase);
		fwrite(ch, sizeof(char), 1, stdout);
		return 1;
	}
	else{
		int count = writeHex(p >> digitLen, digitLen, upperCase);
		ch[0] = writeBitChar(p & (int)(pow(2, digitLen) - 1), upperCase);
		fwrite(ch, sizeof(char), 1, stdout);
		return count + 1;
	}
}
int writeBits(long p, int digitLen){
	if(digitLen<=3){
		return writeHex(p, digitLen, 0);
	}
	return -1;
}
int myPrint(const char *format, va_list arg){
	int i = 0;
	char c;
	ch[1] = '\0';
	while((c = *format++)!= '\0'){
		if (c == '%'){
			char c2 = *format++;
			if(c2 == 'c'){ // Character
				ch[0] = va_arg(arg, int);
				fwrite(ch, sizeof(char), 1, stdout);
				i++;

			}else if(c2 == 'd' || c2 == 'i'){ // Integer
				int d = va_arg(arg, int);
				if(d<0){
					ch[0] = '-';
					fwrite(ch, sizeof(char), 1, stdout);
					i++;
				}
				i += writeInt(abs(d));

			}else if(c2 == 'f'){ // Float/Double
				i += writeFloat(va_arg(arg, double));

			}else if(c2 == 's'){ // String
				char *str = va_arg(arg, char *);
				int size = strlen(str);
				fwrite(str, sizeof(char), size, stdout);
				i+=size;

			}else if(c2 == 'p'){ // Pointer
				ch[0] = '0';
				ch[1] = 'x';
				fwrite(ch, sizeof(char), 2, stdout);
				ch[1] = '\0';
				i+= writeHex(va_arg(arg,unsigned long), 4, 0) + 2;

			}else if(c2 == 'x' || c2 == 'X'){ // Hex
				int upperCase = c2 == 'X'? 1 : 0;
				i+= writeHex(va_arg(arg, unsigned int), 4, upperCase);
			}else if(c2 == 'o'){ // Octal
				i+= writeBits(va_arg(arg, unsigned int), 3);
			} else if(c2 == 'u'){ // Unsigned Decimal
				i += writeInt(va_arg(arg, unsigned int));
			} else{ // Failed to identify or '%'
				ch[0] = '%';
				fwrite(ch, sizeof(char), 1, stdout);
				i++;
			}
		} else{
			ch[0] = c;
			fwrite(ch, sizeof(char), 1, stdout);
			i++;
		}
	}
	printf("\033[0m"); // Reset color
	return i;
}

int infof(const char *format, ...){
	printf("\033[0m"); // Reset color
	va_list arg;
	va_start(arg, format);
	int r = myPrint(format, arg);
	va_end(arg);
	return r;
}

int warnf(const char *format, ...){
	printf("\033[0;33m"); // Set color to yellow
	va_list arg;
	va_start(arg, format);
	int r = myPrint(format, arg);
	va_end(arg);
	return r;
}

int errorf(const char *format, ...){
	printf("\033[0;31m"); // Set color red
	va_list arg;
	va_start(arg, format);
	int r = myPrint(format, arg);
	va_end(arg);
	return r;
}

int panicf(const char *format, ...){
	printf("\033[1;35m"); // Set color to bald magenta
	va_list arg;
	va_start(arg, format);
	myPrint(format, arg);
	va_end(arg);
	abort(); // Cause segmentation fault
	return -1;
}