#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"


void NgxProcessEventsAndTimes()
{
    gSocket.NgxEPollProcessEvents(-1);

    // gSocket.PrintTDInfo();
}