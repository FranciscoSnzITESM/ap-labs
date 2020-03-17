#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "string.h"
#define REPORT_FILE "packages_report.txt"

void analizeLog(char *logFile, char *report);
void writeReport(int wr);

struct Program {
    char* name;
    char* installDate;
    char* updateDate;
    int numUpdates;
    char* removalDate;
};
struct Program programs[600];
int nIns, nUpgr, nCurr;

char* readLine(int rd){
    int size = 50;
    int total = 0;
    char *buffer;
    buffer = calloc(size, sizeof(char));
    int n;
    // Read in buffersized waves
    while((n = read(rd, buffer+total, size)) > 0){
        for(int i=total;i<total+n;i++){
            if(buffer[i] == '\n'){
                // Roll back the offset to after \n
                lseek(rd, i-(total+n)+1, SEEK_CUR);
                buffer[i] = '\0';
                return buffer;
            }
        }
        total+=n;
        buffer = realloc(buffer, total+size);
    }
    if(total>0){
        // If you read something and then encountered EOF send it back
        buffer[total] = '\0';
        return buffer;
    }
    return NULL;
}
char* getWord(char *str){
    int size;
    for(int i=0;i<strlen(str);i++){
        if(str[i] == ' ' || str[i] == '\n'){
            size = i;
            break;
        }
    }
    char *newStr = calloc(size+1, sizeof(char));
    for(int i=0; i<size; i++){
        newStr[i] = str[i];
    }
    newStr[size] = '\0';
    return newStr;
}
char* getDate(char *str){
    int size;
    for(int i=0;i<strlen(str);i++){
        if(str[i] == ']'){
            size = i;
            break;
        }
    }
    char *newStr = calloc(size+1, sizeof(char));
    for(int i=0; i<size; i++){
        newStr[i] = str[i];
    }
    newStr[size] = '\0';
    return newStr;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage:./pacman-analizer.o pacman.log\n");
        return 1;
    }
    analizeLog(argv[1], REPORT_FILE);
    return 0;
}

void newProgram(char* line){
    programs[nIns].name = getWord(line+36);
    programs[nIns].installDate = getDate(line+1);
    nIns++;
    nCurr++;
}
void updateProgram(char* line){
    char *name = getWord(line+35);
    for(int i=0;i<nIns;i++){
        if(strcmp(programs[i].name, name) == 0){
            programs[i].updateDate = getDate(line+1);
            programs[i].numUpdates++;
            nUpgr++;
            free(name);
            return;
        }
    }
    printf("Update Program: Program not found\n");
}
void removeProgram(char* line){
    char *name = getWord(line+34);
    for(int i=0;i<nIns;i++){
        if(strcmp(programs[i].name, name) == 0){
            programs[i].removalDate = getDate(line+1);
            nCurr--;
            free(name);
            return;
        }
    }
    printf("Remove Program: Program not found\n");
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    int rd = open(logFile, O_RDONLY);
    if(rd < 0) {
        printf("Error opening log file\n");
        return;
    }
    int wr = open(report, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(wr < 0) {
        printf("Error opening report file\n");
        return;
    }
    char *line;
    while((line = readLine(rd)) != NULL){
        char *action = getWord(line+26); // Thats where the action word is
        if(strcmp("installed", action) == 0){
            newProgram(line);
        } else if (strcmp("upgraded", action) == 0) {
            updateProgram(line);
        } else if (strcmp("removed", action) == 0) {
            removeProgram(line);
        }
        free(action);
        free(line);
    }

    close(rd);
    writeReport(wr);
    printf("Report is generated at: [%s]\n", report);
}
void writeNumber(int wr, int value){
    char buffer[10];
    int size = sprintf(buffer, "%d\n", value);
    write(wr, buffer, size);
}
void writeProgramInfo(int wr, struct Program p){
    write(wr, "- Package Name        : ", 24);
    write(wr, p.name, strlen(p.name));
    write(wr, "\n  - Install date      : ", 25);
    write(wr, p.installDate, strlen(p.installDate));
    write(wr, "\n  - Last update date  : ", 25);
    if(p.updateDate != NULL){
        write(wr, p.updateDate, strlen(p.updateDate));
    } else{
        write(wr, "-", 1);
    }
    write(wr, "\n  - How many updates  : ", 25);
    writeNumber(wr, p.numUpdates);
    write(wr, "  - Removal date      : ", 24);
    if(p.removalDate != NULL){
        write(wr, p.removalDate, strlen(p.removalDate));
    } else{
        write(wr, "-", 1);
    }
    write(wr, "\n", 1);
}
void writeReport(int wr) {
    write(wr, "Pacman Packages Report\n", 23);
    write(wr, "----------------------\n", 23);    
    write(wr, "- Installed packages : ", 23);
    writeNumber(wr, nIns);
    write(wr, "- Removed packages   : ", 23);
    writeNumber(wr, nIns - nCurr);
    write(wr, "- Upgraded packages  : ", 23);
    writeNumber(wr, nUpgr);
    write(wr, "- Current installed  : ", 23);
    writeNumber(wr, nCurr);

    write(wr, "\nList of packages\n----------------\n", 35);
    for(int i=0;i<nIns;i++){
        writeProgramInfo(wr, programs[i]);
    }
    close(wr);
}

