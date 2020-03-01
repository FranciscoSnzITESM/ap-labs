#define MAXLEN 1000
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void merge(void *lineptr[], int l, int m, int r, int (*comp)(void*, void*)){
	int i,j,k;
	int s1 = m-l+1;
	int s2 = r-m;
	// making copies
	void *left[s1];
	void *right[s2];
	for(i=0; i<s1; i++){
		left[i] = lineptr[l+i];
	}
	for(i=0; i<s2; i++){
		right[i] = lineptr[m+i+1];
	}

	i=0;j=0;k=l;
    while(i<s1 && j<s2){
    	if(comp(left[i],right[j]) < 0){
			lineptr[k++] = left[i++];
		}
		else{
			lineptr[k++] = right[j++];
		}
	}
	// pass what is left
	if(i==s1){
		while(j<s2){
			lineptr[k++] = right[j++];
		}
	}
	else{
		while(i<s1){
			lineptr[k++] = left[i++];
		}
	}
}

void mergeSort(void *lineptr[], int start, int end, int (*comp)(void*, void*)){
	int middle = start+(end-start)/2;
	if(end>start){
		mergeSort(lineptr, start, middle, comp);
		mergeSort(lineptr, middle+1, end, comp);
		merge(lineptr, start, middle, end, comp);
	}
}

int numcmp(char *l, char *r){
	float n1 = atof(l);
	float n2 = atof(r);
	if(n1<n2)
		return -1;
	if(n1==n2)
		return 0;
	return 1;
}

void printArray(char *arr[], int size){
	int i;
	for(i=0;i<size;i++){
        printf("%s ",arr[i]);
    }
    printf("\n");
}

int isNumber(char *value){
	int i;
	for(i=0; i<strlen(value);i++){
		if(isdigit(value[i]) == 0){
			return 0;
		}
	}
	return 1;
}

int main(int argc, char **argv)
{
	if(argc < 2){
		printf("Please insert values\n");
		return -1;
	}
	argv++;
	argc--;
	printf("Original Array\n");
	printArray(argv, argc);
	if(isNumber(argv[0]) == 1){
		printf("The array is numeric\n");
		mergeSort(argv, 0, argc-1, numcmp);
	}
	else{
		printf("The array is made of strings\n");
		mergeSort(argv, 0, argc-1, strcmp);
	}
	printf("\nSorted array\n");
	printArray(argv, argc);
	return 0;
}
