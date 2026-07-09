#include <stdio.h>

#include "ebx_sys.h"
#include "ebx_app.h"

void app_main(void) {
    ebx_disp_init();
    SET_APP_PARAM(test) {
        "test param 0\n",
        NULL
    };
    INIT_APP(test);
}
