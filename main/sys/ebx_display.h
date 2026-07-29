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

typedef enum {
    EBX_DISP_DRAW_MODE_OVERWRITE = 0,
    EBX_DISP_DRAW_MODE_OPTCOLOR,
} ebx_disp_draw_mode_t;

void ebx_disp_draw_at(ebx_disp_draw_mode_t mode, void* buf, int ofs_x, int ofs_y, int width, int height, int param);

#endif /*__INC_EBX_DISPLAY_H__*/
