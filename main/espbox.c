#include <stdio.h>

#include "ebx_app.h"

void app_main(void) {
    SET_APP_PARAM(gnuboy) {
        "/storage/drill.gbc",
        NULL
    };
    INIT_APP(gnuboy);
}
