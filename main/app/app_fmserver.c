#include <stdio.h>
#include <string.h>

#include "ebx_sys.h"
#include "app.h"

#define APP_NAME fmserver
static const char* TAG = "ebx_app_fmserver";

REG_APP {
    printf(params[0]);
}
