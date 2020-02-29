#include <stdio.h>
#include <stdlib.h>
void reverse(int len, char word[]){
	int i;
	char tmp;
	for(i = 0; i < len/2; i++){
		tmp = word[i];
		word[i] = word[len-i-1];
		word[len-i-1] = tmp;
	}
}

int main(){
    // Place your magic here
	char c;
	int i,oldi;
	i = oldi = 0;
	char *word =(char *)malloc(sizeof(char) * 10); //  9 characters + '\0'
	while((c = getchar()) != EOF){
		if(c == '\n'){
			word[i] = '\0';
			reverse(i,word);
			int j;
			for(j=0; j<i;j++){
				printf("%c",word[j]);
			}
			printf("\n");
			free(word);
			oldi = i = 0;
			word =(char *)malloc(sizeof(char) * 10);
		}
		else{
			i++;
			if(i>=oldi+10){
				word = (char *)realloc(word, (i+10)*sizeof(char));
				oldi=i;
			}
			word[i-1] = c;
		}
	}
	free(word);
    return 0;
}
