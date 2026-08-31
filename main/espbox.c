#include <stdio.h>

#include "ebx_sys.h"
#include "ebx_app.h"

void app_main(void) {
    ebx_nvs_init();
    uint8_t pwflags = ebx_nvs_get_u8("power_flags");
    printf("pwflags 0x%x\n", pwflags);
    ebx_nvs_set_u8("power_flags", 1);

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
