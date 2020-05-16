#define _XOPEN_SOURCE 500
#include "logger.h"
#include <limits.h>
#include <ftw.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
int fd;
struct directory{
    int wd;
    char *path;
};
int currDirs;
struct directory **directories;

char *getPwd(const struct inotify_event *event){
    int i;
    for(i = 0; i < currDirs; i++){
        if(directories[i]->wd == event->wd){
            int dirLen = strlen(directories[i]->path);
            char *path = calloc(dirLen + 1 + event->len, sizeof(char));
            strcpy(path, directories[i]->path);
            path[dirLen] = '/';
            strcpy(path + dirLen + 1, event->name);
            return path;
        }
    }
    return NULL;
}

int addDirNotify(const char *path){
    int mask = IN_CREATE | IN_DELETE;
    int wd = inotify_add_watch(fd, path, mask);
    if(wd == -1){
        errorf("Error adding %s to inotify\n", path);
        return -1;
    }
    directories[currDirs] = malloc(sizeof(struct directory));
    directories[currDirs]->wd = wd;
    directories[currDirs]->path = calloc(strlen(path), sizeof(char));
    strcpy(directories[currDirs]->path, path);
    currDirs++;
    return 0;
}

int isDirectory(char *path){
    struct stat buf;
    stat(path, &buf);
    switch (buf.st_mode & S_IFMT) {
    case S_IFDIR:  infof("Directory");               return 1;
    case S_IFIFO:  infof("FIFO/pipe");               break;
    case S_IFLNK:  infof("Symlink");                 break;
    case S_IFREG:  infof("Regular file");            break;
    case S_IFSOCK: infof("Socket");                  break;
    default:       infof("Unknown type");            break;
    }
    return 0;
}

static int treeEntry(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    switch (typeflag) {
    case FTW_D: // Directory
        infof("Directory: %s\n", fpath);
        return addDirNotify(fpath);
    case FTW_DNR: // Directory that can't be read
        errorf("This program doesn't have permissions to read the directory %s\n", fpath);
        return -1;
    case FTW_NS: // Error calling stat(), not symbolic
        warnf("Error calling stat, not a symbolic link\n");
        break;
    }
    return 0;
}

void manageEvent(const struct inotify_event *event){
    if(event->mask & IN_CREATE){
        char *path = getPwd(event);
        int isDir = isDirectory(path);
        infof(" %s created\n", path);
        if(isDir){
            addDirNotify(path);
        }
        free(path);
    }
    else if(event->mask & IN_DELETE){
        char *path = getPwd(event);
        infof("%s deleted\n", path);
    }
    else if(event->mask & IN_MOVED_FROM){

    }
    else if(event->mask & IN_MOVED_TO){

    }
    else {
        warnf("Unknown event %x\n", event->mask);
    }
}

int main(int argc, char *argv[]){
    // Checking parameters
    if (argc != 2){
        errorf("Wrong parameters, usage: %s <directoryPath>\n", argv[0]);
        return -1;
    }
    // NFTW
    fd = inotify_init();
    directories = calloc(10, sizeof(struct directory *));
    if (fd == -1){
        errorf("Error calling inotify_init\n");
        return -1;
    }
    int flags = 0;
    flags |= FTW_PHYS;
    int err = nftw(argv[1], treeEntry, 20, flags);
    if(err){
        errorf("Error calling nftw\n");
    }
    warnf("Configuration is done, listening to changes:\n");
    // Listening to changes
    int n = 0;
    int structSize = sizeof(struct inotify_event);
    int bufSize = structSize + NAME_MAX + 1;
    char *buf = calloc(bufSize, sizeof(char));
    const struct inotify_event *event;
    while (1){
        n = read(fd, buf, bufSize);
        event = (const struct inotify_event *)buf;
        lseek(fd, structSize + event->len - bufSize, SEEK_CUR);
        manageEvent(event);
    }
    return 0;
}
