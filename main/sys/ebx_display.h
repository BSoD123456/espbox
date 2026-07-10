#ifndef __INC_EBX_DISPLAY_H__
#define __INC_EBX_DISPLAY_H__

#define EBX_DISP_FPS        60
#define EBX_DISP_RES_W      160
#define EBX_DISP_RES_H      128

extern void ebx_disp_init(void);
extern void* ebx_disp_render(void);
extern int32_t ebx_disp_wait_frame(uint32_t* p_tick);
extern uint32_t ebx_disp_count_fps(uint32_t tick);

#endif /*__INC_EBX_DISPLAY_H__*/
