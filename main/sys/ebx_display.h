#ifndef __INC_EBX_DISPLAY_H__
#define __INC_EBX_DISPLAY_H__

#define EBX_DISP_FPS        60
#define EBX_DISP_RES_W      160
#define EBX_DISP_RES_H      128

typedef void (*cb_draw_t)(void* buffer);

extern void ebx_disp_init(cb_draw_t cb_draw);

#endif /*__INC_EBX_DISPLAY_H__*/
