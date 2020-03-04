#include <stdio.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char **argv) {
	if(argc!=4){
		printf("Only insert 3 arguments\n");
		return -1;
	}
	printf("Initial Lenght: %d\n", mystrlen(argv[1]));
	char *newStr = mystradd(argv[1],argv[2]);
	printf("New String: %s\n", newStr);
	int found = mystrfind(newStr,argv[3]);
	printf("Substring was found: ");
	if(found<0){
		printf("No\n");
	}
	else{
		printf("Yes, at pos %d\n", found);
	}
	return 0;
}
