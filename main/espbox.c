#include <stdio.h>

#include "ebx_app.h"

void app_main(void) {
    SET_APP_PARAM(gnuboy) {
        "test param 0\n",
        NULL
    };
    INIT_APP(gnuboy);
}
