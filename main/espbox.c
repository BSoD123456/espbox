#include <stdio.h>

#include "ebx_sys.h"
#include "ebx_app.h"

void app_main(void) {
    ebx_fs_init();
    ebx_disp_init();
    ebx_ipt_init();
#if 1
    SET_APP_PARAM(gnuboy) {
        "/storage/siren2.a2.zip",
        "/storage/siren2.sav",
        "/storage/siren2.sta",
        NULL
    };
    INIT_APP(gnuboy);
#endif
}
