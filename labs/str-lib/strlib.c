#include <string.h>
#include <stdlib.h>
int mystrlen(char *str){
	int i;
	for(i=0;str[i]!='\0';i++);
    return i;
}

char *mystradd(char *origin, char *addition){
	int s1 = mystrlen(origin);
	int s2 = mystrlen(addition);
	char *newstr = calloc(s1+s2+1, sizeof(char));
	strcpy(newstr,origin);
	strcpy(newstr+s1,addition);
    return newstr;
}

int mystrfind(char *origin, char *substr){
	int i,j, isFound;
	int s1 = mystrlen(origin);
	int s2 = mystrlen(substr);
	for(i=0;i<s1;i++){
		isFound = 1;
		for(j=0;i+j<s1 && j<s2;j++){
			if(origin[i+j] != substr[j]){
				isFound = 0;
				break;
			}
		}
		if(isFound && j==s2)
			return i;
	}
    return -1;
}
