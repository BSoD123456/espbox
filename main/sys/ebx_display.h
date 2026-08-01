#ifndef __INC_EBX_DISPLAY_H__
#define __INC_EBX_DISPLAY_H__

#define EBX_DISP_FPS        60
#define EBX_DISP_RES_W      160
#define EBX_DISP_RES_H      128

void ebx_disp_init(void);
void* ebx_disp_render_at(int x_start, int y_start, int x_end, int y_end);
int32_t ebx_disp_wait_frame(uint32_t* p_tick);
void ebx_disp_drop_frame(uint32_t* p_tick);
uint32_t ebx_disp_count_fps(uint32_t tick);
uint32_t ebx_disp_count_fps_cur(void);

static inline void* ebx_disp_render(void) {
    return ebx_disp_render_at(0, 0, EBX_DISP_RES_W, EBX_DISP_RES_H);
}

typedef uint16_t ebx_disp_color_t;

#define _EBX_DISP_COLOR_TERM(c, w)          ((uint8_t)(c) >> (8 - (w)))
#define _EBX_DISP_COLOR_RGB565BE(r, g, b)   ((ebx_disp_color_t)((_EBX_DISP_COLOR_TERM(r, 5)<<3) | (_EBX_DISP_COLOR_TERM(b, 5)<<8) | (_EBX_DISP_COLOR_TERM(g, 6)>>3) | (_EBX_DISP_COLOR_TERM(g, 6)<<13)))
#define EBX_DISP_COLOR(r, g, b)             _EBX_DISP_COLOR_RGB565BE(r, g, b)

#define EBX_DISP_DRAW_FLAG_DEFAULT          0x0u
#define EBX_DISP_DRAW_FLAG_OPT_BLACK        0x1u
#define EBX_DISP_DRAW_FLAG_OPT_WHITE        0x2u
#define EBX_DISP_DRAW_FLAG_OPT_PINK         0x3u
#define EBX_DISP_DRAW_FLAG_SWAP             0x4u

void ebx_disp_draw_at(void* buf, int ofs_x, int ofs_y, int width, int height, uint8_t flags);
void ebx_disp_copy_frame(void);

#define EBX_DISP_FCTX_STATIC(vname)     void* vname[2] = {}
void* ebx_disp_fctx_alloc(void);
void ebx_disp_fctx_free(void* fctx);
void* ebx_disp_fctx_swap(void* fctx, void* pval);

#endif /*__INC_EBX_DISPLAY_H__*/
