#include <stdio.h>
#include <string.h>

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

#include "ebx_common.h"
#include "ebx_display.h"

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

#define LOCK_DRAW
//#undef  LOCK_DRAW

static esp_lcd_panel_handle_t g_panel_handle = NULL;

static uint8_t g_disp_buffers[2][DISP_BUFF_SZ] = {};
static uint8_t g_disp_draw_bidx = 0;
#ifdef LOCK_DRAW
static SemaphoreHandle_t g_draw_sem = NULL;
#endif
static SemaphoreHandle_t g_rend_sem = NULL;

static inline void flip_buffer(void) {
    g_disp_draw_bidx = !g_disp_draw_bidx;
}

void* ebx_disp_fctx_alloc(void) {
    return calloc(2, sizeof(void*));
}

void ebx_disp_fctx_free(void* fctx) {
    free(fctx);
}

void* ebx_disp_fctx_peek(void* fctx) {
    return ((void**)fctx)[g_disp_draw_bidx];
}

void* ebx_disp_fctx_swap(void* fctx, void* pval) {
    if(!fctx) {
        ESP_LOGE(TAG, "invalid fctx");
        abort();
    }
    void* r = ((void**)fctx)[g_disp_draw_bidx];
    ((void**)fctx)[g_disp_draw_bidx] = pval;
    return r;
}

void ebx_disp_fctx_foreach(void* fctx, void*(*cb)(void*, void*), void* pctx) {
    ((void**)fctx)[0] = cb(((void**)fctx)[0], pctx);
    ((void**)fctx)[1] = cb(((void**)fctx)[1], pctx);
}

static bool on_flush_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    xSemaphoreGive(g_rend_sem);
    return false;
}

void* ebx_disp_render_at(int x_start, int y_start, int x_end, int y_end) {
    xSemaphoreTake(g_rend_sem, portMAX_DELAY);
#ifdef LOCK_DRAW
    xSemaphoreTake(g_draw_sem, portMAX_DELAY);
#endif
    flip_buffer();
#ifdef LOCK_DRAW
    xSemaphoreGive(g_draw_sem);
#endif
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(g_panel_handle, x_start, y_start, x_end, y_end, g_disp_buffers[!g_disp_draw_bidx]));
    return g_disp_buffers[g_disp_draw_bidx];
}

void ebx_disp_copy_frame(void) {
#ifdef LOCK_DRAW
    xSemaphoreTake(g_draw_sem, portMAX_DELAY);
#endif
    memcpy(g_disp_buffers[g_disp_draw_bidx], g_disp_buffers[!g_disp_draw_bidx], DISP_BUFF_SZ);
#ifdef LOCK_DRAW
    xSemaphoreGive(g_draw_sem);
#endif
}

void ebx_disp_blit_at(void* dbuf, int dwidth, void* buf, int ofs_x, int ofs_y, int width, int height, uint32_t flags) {
    ebx_disp_color_t opt_color = (flags & EBX_DISP_DRAW_FLAG_COLOR_MASK);
    ebx_disp_color_t (*src_buf)[width] = buf;
    ebx_disp_color_t (*dst_buf)[dwidth] = dbuf;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            ebx_disp_color_t c = src_buf[y][x];
            if( (flags & EBX_DISP_DRAW_FLAG_TRANSP) && c == opt_color ) {
                continue;
            }
            if(flags & EBX_DISP_DRAW_FLAG_SWAP) {
                src_buf[y][x] = dst_buf[ofs_y + y][ofs_x + x];
            }
            dst_buf[ofs_y + y][ofs_x + x] = c;
        }
    }
}

void ebx_disp_draw_at(void* buf, int ofs_x, int ofs_y, int width, int height, uint32_t flags) {
#ifdef LOCK_DRAW
    xSemaphoreTake(g_draw_sem, portMAX_DELAY);
#endif
    ebx_disp_blit_at(g_disp_buffers[g_disp_draw_bidx], DISP_RES_LCD_W, buf, ofs_x, ofs_y, width, height, flags);
#ifdef LOCK_DRAW
    xSemaphoreGive(g_draw_sem);
#endif
}

int32_t ebx_disp_wait_frame(uint32_t* p_tick) {
    TickType_t nxt_tick = ((*p_tick * EBX_DISP_FPS * portTICK_PERIOD_MS / 1000 + 1) * 1000 + EBX_DISP_FPS * portTICK_PERIOD_MS - 1) / (EBX_DISP_FPS * portTICK_PERIOD_MS);
    TickType_t cur_tick = xTaskGetTickCount();
    int32_t delt;
    if(nxt_tick > cur_tick) {
        delt = nxt_tick - cur_tick;
        vTaskDelay(delt);
    } else {
        delt = -(int32_t)(cur_tick - nxt_tick);
    }
    *p_tick = nxt_tick;
    return delt;
}

void ebx_disp_drop_frame(uint32_t* p_tick) {
    *p_tick = xTaskGetTickCount();
}

uint32_t ebx_disp_count_fps(uint32_t tick) {
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

uint32_t ebx_disp_count_fps_cur(void) {
    return ebx_disp_count_fps(xTaskGetTickCount());
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
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
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
#ifdef LOCK_DRAW
    g_draw_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(g_draw_sem);
#endif
}

