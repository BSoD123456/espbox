#include <stdio.h>
#include <string.h>

#include "ebx_sys.h"

#include "app.h"

#define APP_NAME gnuboy

static uint8_t g_cnt = 0;
static void do_draw(void* buffer) {
    memset(buffer, g_cnt++, EBX_DISP_RES_W * EBX_DISP_RES_H * 2);
}

REG_APP {
    printf(params[0]);
    ebx_disp_init(&do_draw);
}
