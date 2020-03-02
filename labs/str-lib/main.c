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
	printf("New String: %s\n", mystradd(argv[1],argv[2]));
	printf("Substring was found: %s\n", mystrfind(argv[2],argv[3]) ? "Yes" : "No");
    return 0;
}
