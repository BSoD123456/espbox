#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"

#include "ebx_sys.h"
#include "app.h"

#include "gnuboy/gnuboy.h"

#define APP_NAME    gnuboy
static const char* TAG = "ebx_app_gnuboy";

#define GB_SKIPLINE_CYCLE   (GB_HEIGHT / (GB_HEIGHT - EBX_DISP_RES_H))

static char* g_rom_path = NULL;

static void cb_gb_video(void* buffer) {
    void* nbuf = ebx_disp_render();
    gnuboy_set_framebuffer(nbuf);
}

static void app_task(void* p_param) {
    if(gnuboy_init_custom(GB_PIXEL_565_BE, &cb_gb_video, GB_SKIPLINE_CYCLE) < 0) {
        ESP_LOGE(TAG, "init failed");
        abort();
    }
    if(gnuboy_load_bios_file("/storage/gbc_bios.bin") < 0) {
        ESP_LOGE(TAG, "load bios failed");
        abort();
    }
    if(gnuboy_load_rom_file(g_rom_path) < 0) {
        ESP_LOGE(TAG, "load rom failed");
        abort();
    }
    TickType_t tick = xTaskGetTickCount();
    uint32_t fps = 0;
    bool do_draw = true;
    uint32_t cnt_draw = 0;
    for(;;) {
        gnuboy_run(do_draw);
        if(do_draw) {
            cnt_draw++;
        }
        fps = ebx_disp_count_fps(tick);
        if(fps > 0) {
            ESP_LOGI(TAG, "fps: %lu (%lu)", fps, cnt_draw);
            cnt_draw = 0;
        }
        do_draw = (ebx_disp_wait_frame(&tick) >= 0);
    }
}

REG_APP {
    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, "free: %zu (%zu KB)", free_heap, free_heap / 1024);

    ebx_fs_init();
    ebx_disp_init();
    ebx_input_init();

    g_rom_path = params[0];
    TaskHandle_t hndl_disp = NULL;
    xTaskCreate(app_task, "ebx_app_gnuboy", 0x4000, NULL, 3, &hndl_disp);
}
