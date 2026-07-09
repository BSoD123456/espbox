#include <stdio.h>

#include "ebx_sys.h"

#include "app.h"

#define APP_NAME gnuboy

REG_APP {
    printf(params[0]);
}
