#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"

#include "ebx_display.h"
#include "ebx_ui.h"

//static const char* TAG = "ebx_ui";

struct ebx_ui_win_s {
    void* frame_ctx;
    int width;
    int height;
    ebx_disp_color_t bgcolor;
};

static inline void clean_dbuf(ebx_disp_color_t* wb, ebx_disp_color_t bgc, int sz) {
    for(int i = 0; i < sz; i++) {
        wb[i] = bgc;
    }
}

void* ebx_ui_dbuf_alloc(ebx_ui_win_t* wh, int width, int height) {
    int win_size = width * height;
    ebx_disp_color_t bgc = wh->bgcolor;
    ebx_disp_color_t* wb = (ebx_disp_color_t*)malloc(win_size * sizeof(ebx_disp_color_t));
    clean_dbuf(wb, bgc, win_size);
    return (void*)wb;
}

static void* alloc_wbuf(void* swp, void* pctx) {
    if(swp) {
        return swp;
    }
    ebx_ui_win_t* wh = pctx;
    return ebx_ui_dbuf_alloc(wh, wh->width, wh->height);
};

static void* free_wbuf(void* swp, void* pctx) {
    if(swp) {
        ebx_ui_dbuf_free(swp);
    }
    return NULL;
}

ebx_ui_win_t* ebx_ui_win_alloc(uint8_t tcf) {
    ebx_ui_win_t* wh = calloc(1, sizeof(ebx_ui_win_t));
    wh->bgcolort = ebx_disp_get_transp_color(
    wh->frame_ctx = ebx_disp_fctx_alloc();
    ebx_disp_fctx_foreach(wh->frame_ctx, alloc_wbuf, wh);
}

void ebx_ui_win_free(ebx_ui_win_t* wh) {
    ebx_disp_fctx_foreach(wh->frame_ctx, free_wbuf, NULL);
    ebx_disp_fctx_free(wh->frame_ctx);
    free(wh);
}
