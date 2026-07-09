#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
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

static const char* TAG = "ebx_disp";

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

static esp_lcd_panel_handle_t g_panel_handle = NULL;

static uint8_t g_disp_buffers[2][DISP_BUFF_SZ] = {};
static void* g_draw_buffer = g_disp_buffers[0];
static void* g_rend_buffer = g_disp_buffers[1];
static SemaphoreHandle_t g_rend_sem = NULL;

static inline void flip_buffer(void) {
    void* obuf = g_rend_buffer;
    g_rend_buffer = g_draw_buffer;
    g_draw_buffer = obuf;
}

static bool on_flush_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    xSemaphoreGive(g_rend_sem);
    return false;
}

void* ebx_disp_render(void) {
    xSemaphoreTake(g_rend_sem, portMAX_DELAY);
    flip_buffer();
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(g_panel_handle, 0, 0, EBX_DISP_RES_W, EBX_DISP_RES_H, g_rend_buffer));
    return g_draw_buffer;
}

void ebx_disp_wait_frame(TickType_t* p_tick) {
    vTaskDelay( ((*p_tick * EBX_DISP_FPS * portTICK_PERIOD_MS / 1000 + 1) * 1000 + EBX_DISP_FPS * portTICK_PERIOD_MS - 1) / (EBX_DISP_FPS * portTICK_PERIOD_MS) - *p_tick );
    *p_tick = xTaskGetTickCount();
}

uint32_t ebx_disp_count_fps(TickType_t tick) {
    static int32_t cnt_frame = 0;
    static TickType_t nxt_tick = 0;
    uint32_t r_fps = 0;
    cnt_frame++;
    if(tick >= nxt_tick) {
        if(nxt_tick > 0) {
            r_fps = cnt_frame;
            cnt_frame = 0;
        }
        nxt_tick += 1000 / portTICK_PERIOD_MS;
    }
    return r_fps;
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

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = DISP_PIN_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = DISP_BYTES_PIXEL * 8,
    };

    ESP_LOGI(TAG, "Install ST7735 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7735(io_handle, &panel_config, &g_panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(g_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(g_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(g_panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(g_panel_handle, true, false));

    g_rend_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(g_rend_sem);
}

