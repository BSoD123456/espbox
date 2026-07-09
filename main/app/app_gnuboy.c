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

static SemaphoreHandle_t g_sem_draw_st = NULL;
static SemaphoreHandle_t g_sem_draw_ed = NULL;

static inline void init_sem(void) {
    g_sem_draw_st = xSemaphoreCreateBinary();
    g_sem_draw_ed = xSemaphoreCreateBinary();
}

static void cb_gnu_video(void* buffer) {
    xSemaphoreTake(g_sem_draw_ed, portMAX_DELAY);
    xSemaphoreGive(g_sem_draw_st);
}

static void do_draw(void* buffer) {
    xSemaphoreGive(g_sem_draw_ed);
    xSemaphoreTake(g_sem_draw_st, portMAX_DELAY);
    gnuboy_set_framebuffer(buffer);
}

static char* g_rom_path = NULL;
static void app_task(void* p_param) {
    if(gnuboy_init(0, GB_AUDIO_STEREO_S16, GB_PIXEL_565_BE, &cb_gnu_video, NULL) < 0) {
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
    for(;;) {
        gnuboy_run(true);
    }
}

REG_APP {
    ebx_fs_init();

    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, "free: %zu (%zu KB)", free_heap, free_heap / 1024);

    init_sem();
    ebx_disp_init(&do_draw);
    g_rom_path = params[0];
    TaskHandle_t hndl_disp = NULL;
    xTaskCreate(app_task, "ebx_app_gnuboy", 0x4000, NULL, 3, &hndl_disp);
}
