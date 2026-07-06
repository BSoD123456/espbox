#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"

#include "esp_lcd_st7735.h"

#include "ebx_display.h"

typedef struct {
    esp_lcd_panel_handle_t hndl;
    uint8_t fps;
} task_param_t;

static const char* TAG = "exb_disp";

#define DISP_RES_LCD_W      EBX_DISP_RES_H
#define DISP_RES_LCD_H      EBX_DISP_RES_W
#define DISP_BYTES_PIXEL    2
#define DISP_BUFF_SZ        (DISP_RES_LCD_W * DISP_RES_LCD_H * DISP_BYTES_PIXEL)

#define DISP_SPIHOST_LCD    2
#define DISP_PCLK_LCD       (40 * 1000 * 1000)

#define DISP_PIN_SCLK       18
#define DISP_PIN_MOSI       23
#define DISP_PIN_MISO       19
#define DISP_PIN_LCD_DC     4
#define DISP_PIN_LCD_RST    19
#define DISP_PIN_LCD_CS     5

#define DISP_BITS_CMD       8
#define DISP_BITS_PARAM     8


static uint8_t g_disp_buffers[2][DISP_BUFF_SZ];
static void* g_draw_buffer = g_disp_buffers[0];
static void* g_rend_buffer = g_disp_buffers[1];

static inline void flip_buffer() {
    void* obuf = g_rend_buffer;
    g_rend_buffer = g_draw_buffer;
    g_draw_buffer = obuf;
}

static SemaphoreHandle_t sem_rend_sync = NULL;
static SemaphoreHandle_t sem_draw_sync = NULL;

static bool on_flush_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    xSemaphoreGive(sem_rend_sync);
    return false;
}

static uint8_t g_cnt_fps;
static TickType_t g_cnt_dtick;
static void do_task(void* p_param) {
    const task_param_t* p_tsk_param = (task_param_t*)p_param;
    const TickType_t step_tick = 1000 / p_tsk_param->fps / portTICK_PERIOD_MS; 
    TickType_t cur_tick = xTaskGetTickCount();
    g_cnt_fps = 0;
    g_cnt_dtick = cur_tick;

    vSemaphoreCreateBinary(sem_rend_sync);
    vSemaphoreCreateBinary(sem_draw_sync);
    for(;;) {
        xSemaphoreTake(sem_draw_sync, portMAX_DELAY);
        cur_tick = xTaskGetTickCount();
        g_cnt_fps++;
        if(cur_tick >= g_cnt_dtick) {
            ESP_LOGI(TAG, "fps: %d", g_cnt_fps);
            g_cnt_dtick += 1000 / portTICK_PERIOD_MS;
            g_cnt_fps = 0;
        }
        flip_buffer();
        esp_lcd_panel_draw_bitmap(p_tsk_param->hndl, 0, 0, EBX_DISP_RES_H, EBX_DISP_RES_W, g_rend_buffer);
        xSemaphoreTake(sem_rend_sync, portMAX_DELAY);
    }
}

void ebx_disp_draw_done(void) {
    if(sem_draw_sync) {
        xSemaphoreGive(sem_draw_sync);
    }
}

void ebx_disp_init(void) {
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = DISP_PIN_SCLK,
        .mosi_io_num = DISP_PIN_MOSI,
        .miso_io_num = DISP_PIN_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = DISP_BUFF_SZ
    };
    ESP_ERROR_CHECK(spi_bus_initialize(DISP_SPIHOST_LCD, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = DISP_PIN_LCD_DC,
        .cs_gpio_num = DISP_PIN_LCD_CS,
        .pclk_hz = DISP_PCLK_LCD,
        .lcd_cmd_bits = DISP_BITS_CMD,
        .lcd_param_bits = DISP_BITS_PARAM,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = on_flush_done,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)DISP_SPIHOST_LCD, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = DISP_PIN_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = DISP_BYTES_PIXEL * 8,
    };

    ESP_LOGI(TAG, "Install ST7735 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7735(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    //ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
    //ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));

    memset(g_draw_buffer, 0xaa, DISP_BUFF_SZ);
    memset(g_rend_buffer, 0x55, DISP_BUFF_SZ);

    static task_param_t tsk_param = {};
    tsk_param.hndl = panel_handle,
    tsk_param.fps = EBX_DISP_FPS;
    TaskHandle_t hndl_disp = NULL;
    xTaskCreate(do_task, "display", 0x1000, (void*)&tsk_param, 3, &hndl_disp);
}

