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
    int ofs_x;
    int ofs_y;
    uint32_t draw_flags;
};

static inline void clean_buf(ebx_disp_color_t* wb, ebx_disp_color_t bgc, int sz) {
    for(int i = 0; i < sz; i++) {
        wb[i] = bgc;
    }
}

static void* alloc_wbuf(void* swp, void* pctx) {
    if(swp) {
        return swp;
    }
    ebx_ui_win_t* wh = pctx;
    int win_size = wh->width * wh->height;
    ebx_disp_color_t bgc = wh->bgcolor;
    ebx_disp_color_t* wb = (ebx_disp_color_t*)malloc(win_size * sizeof(ebx_disp_color_t));
    clean_buf(wb, bgc, win_size);
    return (void*)wb;
};

static void* free_wbuf(void* swp, void* pctx) {
    if(swp) {
        free(swp);
    }
    return NULL;
}

ebx_ui_win_t* ebx_ui_win_create(int width, int height, ebx_disp_color_t bgcolor, uint32_t draw_flags) {
    ebx_ui_win_t* wh = calloc(1, sizeof(ebx_ui_win_t));
    wh->width = width;
    wh->height = height;
    wh->bgcolor = bgcolor;
    wh->draw_flags = EBX_DISP_DRAW_FLAGS(draw_flags & ~EBX_DISP_DRAW_FLAG_SWAP, bgcolor);
    wh->frame_ctx = ebx_disp_fctx_alloc();
    ebx_disp_fctx_foreach(wh->frame_ctx, alloc_wbuf, wh);
    return wh;
}

void ebx_ui_win_destroy(ebx_ui_win_t* wh) {
    ebx_disp_fctx_foreach(wh->frame_ctx, free_wbuf, NULL);
    ebx_disp_fctx_free(wh->frame_ctx);
    free(wh);
}

void ebx_ui_win_move_to(ebx_ui_win_t* wh, int x, int y) {
    wh->ofs_x = x;
    wh->ofs_y = y;
}

void ebx_ui_win_draw(ebx_ui_win_t* wh, const void* buf, int ofs_x, int ofs_y, int width, int height) {
    void* wbuf = ebx_disp_fctx_peek(wh->frame_ctx);
    ebx_disp_blit_at(wbuf, wh->width, wh->height, (void*)buf, ofs_x, ofs_y, width, height, wh->draw_flags);
}

void ebx_ui_win_erase(ebx_ui_win_t* wh, int ofs_x, int ofs_y, int width, int height) {
    void* wbuf = ebx_disp_fctx_peek(wh->frame_ctx);
    ebx_disp_blit_at(wbuf, wh->width, wh->height, NULL, ofs_x, ofs_y, width, height, (wh->draw_flags & ~EBX_DISP_DRAW_FLAG_TRANSP) | EBX_DISP_DRAW_FLAG_FILL);
}

void ebx_ui_win_clean(ebx_ui_win_t* wh) {
    void* wbuf = ebx_disp_fctx_peek(wh->frame_ctx);
    clean_buf((ebx_disp_color_t*)wbuf, wh->bgcolor, wh->width * wh->height);
}

typedef struct {
    int win_width;
    int win_height;
    const void* buf;
    int ofs_x;
    int ofs_y;
    int width;
    int height;
    uint32_t flags;
} blit_param_t;

static void* blit_wbuf(void* swp, void* pctx) {
    blit_param_t* pparam = pctx;
    ebx_disp_blit_at(swp, pparam->win_width, pparam->win_height, (void*)pparam->buf, pparam->ofs_x, pparam->ofs_y, pparam->width, pparam->height, pparam->flags);
    return swp;
}

typedef struct {
    ebx_disp_color_t bgcolor;
    int size;
} clean_param_t;

static void* clean_wbuf(void* swp, void* pctx) {
    clean_param_t* pparam = pctx;
    clean_buf((ebx_disp_color_t*)swp, pparam->bgcolor, pparam->size);
    return swp;
}

void ebx_ui_win_draw_each(ebx_ui_win_t* wh, const void* buf, int ofs_x, int ofs_y, int width, int height) {
    blit_param_t param = {
        .win_width = wh->width,
        .win_height = wh->height,
        .buf = buf,
        .ofs_x = ofs_x,
        .ofs_y = ofs_y,
        .width = width,
        .height = height,
        .flags = wh->draw_flags,
    };
    ebx_disp_fctx_foreach(wh->frame_ctx, blit_wbuf, &param);
}

void ebx_ui_win_erase_each(ebx_ui_win_t* wh, int ofs_x, int ofs_y, int width, int height) {
    blit_param_t param = {
        .win_width = wh->width,
        .win_height = wh->height,
        .buf = NULL,
        .ofs_x = ofs_x,
        .ofs_y = ofs_y,
        .width = width,
        .height = height,
        .flags = ((wh->draw_flags & ~EBX_DISP_DRAW_FLAG_TRANSP) | EBX_DISP_DRAW_FLAG_FILL),
    };
    ebx_disp_fctx_foreach(wh->frame_ctx, blit_wbuf, &param);
}

void ebx_ui_win_clean_each(ebx_ui_win_t* wh) {
    clean_param_t param = {
        .bgcolor = wh->bgcolor,
        .size = wh->width * wh->height,
    };
    ebx_disp_fctx_foreach(wh->frame_ctx, clean_wbuf, &param);
}

void ebx_ui_win_swap(ebx_ui_win_t* wh) {
    void* wbuf = ebx_disp_fctx_peek(wh->frame_ctx);
    ebx_disp_draw_at(wbuf, wh->ofs_x, wh->ofs_y, wh->width, wh->height, wh->draw_flags | EBX_DISP_DRAW_FLAG_SWAP);
}
