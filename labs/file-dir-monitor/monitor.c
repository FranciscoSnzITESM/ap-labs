#define _XOPEN_SOURCE 500
#include <stdio.h>
#include "logger.h"
#include <ftw.h>

static int treeEntry(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    switch (typeflag)
    {
    case FTW_F: // Regular file
        infof("Regular file: %s\n", fpath);
        break;
    case FTW_D: // Directory
        infof("Directory: %s\n", fpath);
        break;
    case FTW_DNR: // Directory that can't be read
        errorf("This program doesn't have permissions to read the directory %s\n", fpath);
        return -1;
    case FTW_NS: // Error calling stat(), not symbolic
        warnf("Error calling stat, not a symbolic link\n");
        break;
    case FTW_SL:
        infof("Symbolic link, not following it\n");
        break;
    default:
        warnf("Unknown flag\n");
        break;
    }
    return 0;
}

int main(int argc, char *argv[]){
    if (argc != 2){
        errorf("Wrong parameters, usage: %s <directoryPath>\n", argv[0]);
        return -1;
    }
    int flags = 0;
    flags |= FTW_PHYS;
    int err = nftw(argv[1], treeEntry, 20, flags);
    return 0;
}
