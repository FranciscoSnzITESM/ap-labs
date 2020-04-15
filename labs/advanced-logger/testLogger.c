#include <stdio.h>
#include "logger.h"

int main(){

    // default logging
    infof("INFO Message %d\n", 1);
    warnf("WARN Message %d\n", 2);
    errorf("ERROR Message %d\n", 2);

    // stdout logging
    initLogger("stdout");
    infof("INFO Message %d\n", 1);
    warnf("WARN Message %d\n", 2);
    errorf("ERROR Message %d\n", 2);

    // syslog logging
    initLogger("syslog");
    infof("INFO Message %d\n", 1);
    warnf("WARN Message %d\n", 2);
    errorf("ERROR Message %d\n", 2);

    return 0;
}
