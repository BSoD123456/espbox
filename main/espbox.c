#include <stdio.h>

#include "sys/ebx_sys.h"
#include "app/ebx_app.h"

void app_main(void) {
    ebx_disp_init();
    SET_APP_PARAM(test) {
        "test param 0\n",
        NULL
    };
    INIT_APP(test);
}
