#include <stdio.h>

#include "ebx_app.h"

void app_main(void) {
    SET_APP_PARAM(gnuboy) {
        "/storage/siren2.zip",
        NULL
    };
    INIT_APP(gnuboy);
}
