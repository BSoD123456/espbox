#include <stdio.h>

#include "ebx_sys.h"
#include "app.h"

#define APP_NAME test

REG_APP {
    printf(params[0]);

    ebx_fs_init();
}
