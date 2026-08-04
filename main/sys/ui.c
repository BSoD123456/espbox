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

#define WIN_SIZE(wbp)       ( ((ebx_ui_win_t*)(wbp))->width * ((ebx_ui_win_t*)(wbp))->height )

static void* alloc_wbuf(void* swp, void* pctx) {
    if(swp) {
        return swp;
    }
    int win_size = WIN_SIZE(pctx);
    ebx_disp_color_t bgc = ((ebx_ui_win_t*)pctx)->bgcolor;
    ebx_disp_color_t* wb = (ebx_disp_color_t*)malloc(win_size * sizeof(bgc));
    for(int i = 0; i < win_size; i++) {
        wb[i] = bgc;
    }
    return (void*)wb;
};

static void* free_wbuf(void* swp, void* pctx) {
    if(swp) {
        free(swp);
    }
    return NULL;
}

ebx_ui_win_t* ebx_ui_win_alloc(void) {
    ebx_ui_win_t* wh = calloc(1, sizeof(ebx_ui_win_t));
    wh->frame_ctx = ebx_disp_fctx_alloc();
    ebx_disp_fctx_foreach(wh->frame_ctx, alloc_wbuf, wh);
}

void ebx_ui_win_free(ebx_ui_win_t* wh) {
    ebx_disp_fctx_foreach(wh->frame_ctx, free_wbuf, NULL);
    ebx_disp_fctx_free(wh->frame_ctx);
    free(wh);
}
