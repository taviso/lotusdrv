#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <conio.h>
#include <dos.h>

#include "config.h"
#include "debug.h"
#include "lottypes.h"
#include "idatypes.h"
#include "lotcalls.h"
#include "bundle.h"

int ParseConfig(struct BDLHDR far *bdlptr)
{
    trace("ParseVbdConfig");

    //callbacks->MapMemVmr(bdlptr, 0);
    //callbacks->LoadVmr(0);

    traceint(bdlptr->bdllen);
    traceint(bdlptr->bdlver);
    traceint(bdlptr->driver_id);
    traceint(bdlptr->bundle_id);

    tracebuf(bdlptr, bdlptr->bdllen);

    return 0;
}
