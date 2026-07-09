#ifndef __INC_EBX_DISPLAY_H__
#define __INC_EBX_DISPLAY_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define EBX_DISP_FPS        30
#define EBX_DISP_RES_W      160
#define EBX_DISP_RES_H      128

extern void ebx_disp_init(void);
extern void* ebx_disp_render(void);
extern void ebx_disp_wait_frame(TickType_t* p_tick);
extern uint32_t ebx_disp_count_fps(TickType_t tick);

#endif /*__INC_EBX_DISPLAY_H__*/
