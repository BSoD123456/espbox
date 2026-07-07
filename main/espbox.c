#include <stdio.h>

#include "sys/ebx_sys.h"
#include "app/ebx_app.h"

void app_main(void) {
    ebx_disp_init();
    INIT_APP(test);
}
