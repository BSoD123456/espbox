#include <stdio.h>

#include "ebx_app.h"

void app_main(void) {
    SET_APP_PARAM(gnuboy) {
        "/storage/siren2_cn.gbc",
        NULL
    };
    INIT_APP(gnuboy);
}
