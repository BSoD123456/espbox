#include <stdio.h>

#include "esp_system.h"

#include "ebx_sys.h"
#include "ebx_app.h"

static RTC_DATA_ATTR struct {
    uint32_t magic;
    uint32_t data;
    uint32_t checksum;
} rtc_data;

#define RTC_MAGIC   0x2e727463u

static uint32_t rtc_check(void) {
    uint32_t rdata = (uint32_t)-1;
    printf("magic %lx data %lx chk %lx\n", rtc_data.magic, rtc_data.data, rtc_data.checksum);
    if(rtc_data.magic == RTC_MAGIC && rtc_data.magic + rtc_data.data == rtc_data.checksum) {
        rdata = rtc_data.data++;
    } else {
        rtc_data.magic = RTC_MAGIC;
        rtc_data.data = 0;
    }
    rtc_data.checksum = rtc_data.magic + rtc_data.data;
    return rdata;
}

void app_main(void) {
    uint32_t rr = rtc_check();

    ebx_fs_init();
    ebx_disp_init();
    ebx_ipt_init();
#if 0
    SET_APP_PARAM(gnuboy) {
        "/storage/siren2.a2.zip",
        "/storage/siren2.sav",
        "/storage/siren2.sta",
        NULL
    };
    INIT_APP(gnuboy);
#endif
}
