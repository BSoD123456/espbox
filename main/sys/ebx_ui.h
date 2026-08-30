#ifndef __INC_EBX_UI_H__
#define __INC_EBX_UI_H__

typedef struct ebx_ui_win_s ebx_ui_win_t

ebx_ui_win_t* ebx_ui_win_create(int width, int height, ebx_disp_color_t bgcolor, uint32_t draw_flags);
void ebx_ui_win_destroy(ebx_ui_win_t* wh);
void ebx_ui_win_move_to(ebx_ui_win_t* wh, int x, int y);

void ebx_ui_win_draw(ebx_ui_win_t* wh, void* buf, int ofs_x, int ofs_y, int width, int height);
void ebx_ui_win_erase(ebx_ui_win_t* wh, int ofs_x, int ofs_y, int width, int height);
void ebx_ui_win_clean(ebx_ui_win_t* wh);
void ebx_ui_win_draw_each(ebx_ui_win_t* wh, void* buf, int ofs_x, int ofs_y, int width, int height);
void ebx_ui_win_erase_each(ebx_ui_win_t* wh, int ofs_x, int ofs_y, int width, int height);
void ebx_ui_win_clean_each(ebx_ui_win_t* wh);

void ebx_ui_win_swap(ebx_ui_win_t* wh);

#endif /*__INC_EBX_UI_H__*/
