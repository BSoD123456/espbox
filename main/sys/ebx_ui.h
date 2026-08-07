#ifndef __INC_EBX_UI_H__
#define __INC_EBX_UI_H__

typedef struct ebx_ui_win_s ebx_ui_win_t

ebx_ui_win_t* ebx_ui_win_alloc(void);
void ebx_ui_win_free(ebx_ui_win_t* wh);

void* ebx_ui_dbuf_alloc(ebx_ui_win_t* wh, int width, int height);
static inline void* ebx_ui_dbuf_free(void* dbuf) {
    free(dbuf);
}

#endif /*__INC_EBX_UI_H__*/
