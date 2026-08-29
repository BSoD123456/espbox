#ifndef __INC_GNUBOY_MENU_H__
#define __INC_GNUBOY_MENU_H__

#define MENU_ICON_NUM_W         3
#define MENU_ICON_NUM_H         2
#define MENU_ICON_NUM           ( MENU_ICON_NUM_W * MENU_ICON_NUM_H )

#define MENU_ICON_SIZE_W        8
#define MENU_ICON_SIZE_H        8

#define MENU_ICON_ARROW_SIZE_W  4
#define MENU_ICON_ARROW_SIZE_H  7

#define MENU_UNIT_SIZE_W        ( MENU_ICON_SIZE_W + MENU_ICON_ARROW_SIZE_W )
#define MENU_UNIT_SIZE_H        MENU_ICON_SIZE_H

#define MENU_SIZE_W             ( MENU_ICON_NUM_W * MENU_UNIT_SIZE_W )
#define MENU_SIZE_H             ( MENU_ICON_NUM_H * MENU_UNIT_SIZE_H )

#define MENU_COLOR_MAGICPINK    EBX_DISP_COLOR(255, 0, 255)

#define C0                      MENU_COLOR_MAGICPINK
#define CW                      EBX_DISP_COLOR(255, 255, 255)
#define CB                      EBX_DISP_COLOR(0, 0, 0)

static const ebx_ui_win_t menu_icon_empty[MENU_ICON_SIZE_H][MENU_ICON_SIZE_W] = {
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
};

static const ebx_ui_win_t menu_icon_key_sel[MENU_ICON_SIZE_H][MENU_ICON_SIZE_W] = {
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, CW, CW, CW, CW, C0, C0  },
    {   C0, CW, CB, CB, CB, CB, CW, C0  },
    {   C0, CW, CB, CB, CB, CB, CW, C0  },
    {   C0, C0, CW, CW, CW, CW, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
    {   C0, C0, C0, C0, C0, C0, C0, C0  },
};

static const ebx_ui_win_t menu_icon_key_start[MENU_ICON_SIZE_H][MENU_ICON_SIZE_W] = {
    {   C0, CW, C0, C0, C0, C0, C0, C0  },
    {   CW, CB, CW, CW, C0, C0, C0, C0  },
    {   CW, CB, CB, CB, CW, CW, C0, C0  },
    {   CW, CB, CB, CB, CB, CB, CW, C0  },
    {   CW, CB, CB, CB, CB, CB, CB, CW  },
    {   CW, CB, CB, CB, CB, CW, CW, C0  },
    {   CW, CB, CB, CW, CW, C0, C0, C0  },
    {   C0, CW, CW, C0, C0, C0, C0, C0  },
};

static const ebx_ui_win_t menu_icon_arrow[MENU_ICON_ARROW_SIZE_H][MENU_ICON_ARROW_SIZE_W] = {
    {   C0, CW, C0, C0  },
    {   CW, CB, CW, C0  },
    {   CW, CB, CB, CW  },
    {   CW, CB, CB, CB  },
    {   CW, CB, CB, CW  },
    {   CW, CB, CW, C0  },
    {   C0, CW, C0, C0  },
};

#undef C0
#undef CW
#undef CB

static const void* menu_icons[MENU_ICON_NUM_H][MENU_ICON_NUM_W] = {
    {   menu_icon_key_sel,      menu_icon_key_start,    menu_icon_empty,        },
    {   menu_icon_empty,        menu_icon_empty,        menu_icon_empty,        },
};

static ebx_ui_win_t* g_menu_win = NULL;
static int g_menu_sidx = 0;
static int g_menu_sidx_drawn = 0;

static void _draw_icons(void) {
    for(int row = 0; row < MENU_ICON_NUM_H; row++) {
        int y = MENU_UNIT_SIZE_H * row;
        for(int col = 0; col < MENU_ICON_NUM_W; col++) {
            int x = MENU_UNIT_SIZE_W * col;
            ebx_ui_win_draw(g_menu_win, menu_icons[row][col], x + MENU_ICON_ARROW_SIZE_W, y, MENU_ICON_SIZE_W, MENU_ICON_SIZE_H);
        }
    }
}

static void _update_arrow(void) {
    if(g_menu_sidx_drawn == g_menu_sidx) {
        return;
    }
    int sel_row = g_menu_sidx_drawn / MENU_ICON_NUM_W;
    int sel_col = g_menu_sidx_drawn % MENU_ICON_NUM_W;
    ebx_ui_win_erase(g_menu_win, MENU_UNIT_SIZE_W * sel_col, MENU_UNIT_SIZE_H * sel_col, MENU_ICON_ARROW_SIZE_W, MENU_ICON_ARROW_SIZE_H);
    sel_row = g_menu_sidx / MENU_ICON_NUM_W;
    sel_col = g_menu_sidx % MENU_ICON_NUM_W;
    ebx_ui_win_draw(g_menu_win, menu_icon_arrow, MENU_UNIT_SIZE_W * sel_col, MENU_UNIT_SIZE_H * sel_col, MENU_ICON_ARROW_SIZE_W, MENU_ICON_ARROW_SIZE_H);
    g_menu_sidx_drawn = g_menu_sidx;
}

static void menu_sel_to(int sidx) {
    sidx %= MENU_ICON_NUM;
    if(sidx == g_menu_sidx) {
        return;
    }
    g_menu_sidx = sidx;
}

static inline void menu_sel_by(int dcol, int drow) {
    menu_sel_to(drow * MENU_ICON_NUM_W + dcol);
}

static void menu_update(void) {
    ebx_disp_render();
    ebx_ui_win_swap();
    _update_arrow();
    ebx_ui_win_swap();
}

static void menu_init(void) {
    if(g_menu_win) {
        ESP_LOGE(TAG, "re-init menu");
        abort();
    }
    g_menu_win = ebx_ui_win_create(MENU_SIZE_W, MENU_SIZE_H, MENU_COLOR_MAGICPINK, EBX_DISP_DRAW_FLAG_TRANSP);
    g_menu_sidx = 0;
    _draw_icons();
}

static void menu_deinit(void) {
    if(g_menu_win) {
        ebx_ui_win_destroy(g_menu_win);
        g_menu_win = NULL;
    }
}

#else
#error duplicated include
#endif /*__INC_GNUBOY_MENU_H__*/
