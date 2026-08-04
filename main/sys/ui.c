#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"

#include "ebx_display.h"
#include "ebx_ui.h"

//static const char* TAG = "ebx_ui";

typedef struct {
    int width;
    int height;
    ebx_disp_color_t bgcolor;
} win_buf_param_t;

#define WIN_SIZE(wbp)       ( ((win_buf_param_t*)(wbp))->width * ((win_buf_param_t*)(wbp))->height )

struct ebx_ui_win_s {
    void* frame_ctx;
    win_buf_param_t wb_param;
};

static void* alloc_wbuf(void* swp, void* pctx) {
    if(swp) {
        return swp;
    }
    int win_size = WIN_SIZE(pctx);
    ebx_disp_color_t bgc = ((win_buf_param_t*)pctx)->bgcolor;
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
    ebx_disp_fctx_foreach(wh->frame_ctx, alloc_wbuf, &wh->wb_param);
}

void ebx_ui_win_free(ebx_ui_win_t* wh) {
    ebx_disp_fctx_foreach(wh->frame_ctx, free_wbuf, &wh->wb_param);
    ebx_disp_fctx_free(wh->frame_ctx);
    free(wh);
}
