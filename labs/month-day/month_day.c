#include <stdio.h>
#include <stdlib.h>
/* month_day function's prototype*/
void month_day(int year, int yearday, int *pmonth, int *pday){
	static char daytab[2][13] = {
    	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};
	int leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	if(yearday > 365 + leap){
		printf("%d has %d days, your yearday: %d\n",year,365+leap,yearday);
		return;
	}
	int i = 0;
	while(yearday - daytab[leap][++i] > 0){
		yearday-=daytab[leap][i];
	}
	*pmonth = i;
	*pday = yearday;
}
char *getmonth(int n){
	static char *name[] = {
       "Wrong values",
       "January", "February", "March",
       "April", "May", "June",
       "July", "August", "September",
       "October", "November", "December"
   };
	return (n<1 || n>12) ? name[0] : name[n];
}
int main(int argc, char **argv) {
	if(argc!=3){
		printf("You have to insert year and yearday parameters\n");
		return -1;
	}
	int year = atoi(argv[1]);
	int yearday = atoi(argv[2]);
	if(year <= 0 || yearday <= 0){
		printf("Year and Yearday have to be higher than 0\n");
		return -1;
	}
	int month, day;
	month_day(year, yearday, &month, &day);
	printf("%s %02d, %d\n", getmonth(month), day, year);
    return 0;
}
