#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"

#include "ebx_sys.h"
#include "app.h"

#include "gnuboy/gnuboy.h"

#define APP_NAME    gnuboy
static const char* TAG = "ebx_app_gnuboy";

//#define GB_DBG_LOG
#undef GB_DBG_LOG
#ifdef GB_DBG_LOG
#define DBG_LOGI            ESP_LOGI
#else
#define DBG_LOGI(...)
#endif

#include "gnuboy_cart_exmem.h"
#include "gnuboy_menu.h"

#define GB_SKIPLINE_CYCLE   (GB_HEIGHT / (GB_HEIGHT - EBX_DISP_RES_H))

#define DISP_MIN_FPS 15

static const char* g_rom_path = NULL;
static const char* g_sram_path = NULL;
static const char* g_stat_path = NULL;

static int  g_menuconf_key = 0;
static int  g_menuconf_pad = 0;
static int  g_menuconf_pad_kpdef = 0;
static int  g_menuconf_keypress = 0;

typedef enum {
    MENU_KEY_STAT_IDLE = 0,
    MENU_KEY_STAT_MENU,
    MENU_KEY_STAT_PRESS,
} menu_key_stat_t;

static void cb_gb_video(void* buffer) {
    void* nbuf = ebx_disp_render();
    gnuboy_set_framebuffer(nbuf);
}

static void app_task(void* p_param) {
    if(gnuboy_init_custom(GB_PIXEL_565_BE, &cb_gb_video, GB_SKIPLINE_CYCLE) < 0) {
        ESP_LOGE(TAG, "init failed");
        abort();
    }
    if(gnuboy_load_bios_file("/storage/gbc_bios.bin") < 0) {
        ESP_LOGE(TAG, "load bios failed");
        abort();
    }
    //cart_load_rom_from_file(g_rom_path);
    cart_load_rom_from_zip_file(g_rom_path);
    gnuboy_reset(true);
    if(gnuboy_load_sram(g_sram_path)) {
        ESP_LOGW(TAG, "load sram failed: %s", g_sram_path);
    }

    ebx_nvs_set_u8("power_flags", 0);
    TickType_t tick = xTaskGetTickCount();
    uint32_t fps = 0;
    bool do_draw = true;
    uint32_t cnt_draw = 0;
    uint32_t last_keys = 0;
    uint32_t cnt_skip = 0;
    bool last_sram_dirty = false;
    bool menu_disp = false;
    int menu_keypress_cnt = -1;
    menu_key_stat_t menu_key_stat = MENU_KEY_STAT_IDLE;
    int menu_keypress_pad = g_menuconf_pad_kpdef;
    int menu_exhausted = 0;
    menu_init();
    menu_move_to(120, 5);
    for(;;) {
        uint32_t keys = ebx_ipt_get();
        if(menu_exhausted > 0) menu_exhausted--;
        if(keys != last_keys) {
            last_keys = keys;
            if(menu_disp) {
                if(EBX_IPT_CHK_KEYS(keys, g_menuconf_key)) {
                    int sidx = menu_get_sel();
                    DBG_LOGI(TAG, "menu sel %d", sidx);
                    if(sidx < 3) {
                        switch(sidx) {
                        case 0:
                            menu_keypress_pad = GB_PAD_SELECT;
                            break;
                        case 1:
                            menu_keypress_pad = GB_PAD_START;
                            break;
                        case 2:
                            menu_keypress_pad = g_menuconf_pad;
                            break;
                        }
                        menu_keypress_cnt = 0;
                        menu_key_stat = MENU_KEY_STAT_PRESS;
                    } else {
                        switch(sidx) {
                        case 3:
                            gnuboy_reset(false);
                            break;
                        case 4:
                            gnuboy_load_state(g_stat_path);
                            break;
                        case 5:
                            gnuboy_save_state(g_stat_path);
                            break;
                        }
                        menu_key_stat = MENU_KEY_STAT_IDLE;
                        menu_exhausted = 20;
                    }
                    menu_disp = false;
                    menu_clean();
                    DBG_LOGI(TAG, "menu close");
                } else if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_UP)) {
                    menu_sel_by(0, -1);
                } else if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_DOWN)) {
                    menu_sel_by(0, 1);
                } else if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_LEFT)) {
                    menu_sel_by(-1, 0);
                } else if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_RIGHT)) {
                    menu_sel_by(1, 0);
                } else if(keys) {
                    menu_disp = false;
                    menu_clean();
                    DBG_LOGI(TAG, "menu close");
                    menu_key_stat = MENU_KEY_STAT_IDLE;
                    menu_exhausted = 20;
                }
            }
            if(!menu_disp && menu_exhausted <= 0) {
                int pad = 0;
                if(EBX_IPT_CHK_KEYS(keys, g_menuconf_key)) {
                    if(menu_keypress_cnt < 0) {
                        menu_keypress_cnt = 0;
                    }
                    pad |= menu_keypress_pad;
                } else if(menu_keypress_cnt >= 0) {
                    if(menu_key_stat == MENU_KEY_STAT_IDLE) {
                        pad |= g_menuconf_pad;
                        EBX_IPT_SET_KEYS(last_keys, g_menuconf_key);
                    }
                    menu_keypress_cnt = -1;
                    menu_keypress_pad = g_menuconf_pad_kpdef;
                    menu_key_stat = MENU_KEY_STAT_IDLE;
                }
                if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_UP)) pad |= GB_PAD_UP;
                if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_DOWN)) pad |= GB_PAD_DOWN;
                if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_LEFT)) pad |= GB_PAD_LEFT;
                if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_RIGHT)) pad |= GB_PAD_RIGHT;
                if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_A) && g_menuconf_key != EBX_IPT_KEY_A) pad |= GB_PAD_A;
                if(EBX_IPT_CHK_KEYS(keys, EBX_IPT_KEY_B) && g_menuconf_key != EBX_IPT_KEY_B) pad |= GB_PAD_B;
                gnuboy_set_pad(pad);
                DBG_LOGI(TAG, "pad 0x%x", pad);
            }
        }
        if(menu_keypress_cnt >= 0) {
            if(menu_key_stat == MENU_KEY_STAT_IDLE) {
                if(menu_keypress_cnt++ >= g_menuconf_keypress) {
                    menu_disp = true;
                    ebx_disp_copy_frame();
                    menu_sel_to(1, 0);
                    DBG_LOGI(TAG, "menu open");
                    menu_keypress_cnt = -1;
                    menu_key_stat = MENU_KEY_STAT_MENU;
                }
            }
        }
        if(menu_disp) {
            if(do_draw) {
                menu_update();
            }
        } else {
            gnuboy_run(do_draw);
        }
        if(do_draw) {
            cnt_draw++;
            cnt_skip = 0;
        } else {
            cnt_skip++;
#ifdef DISP_MIN_FPS
            if(cnt_skip > EBX_DISP_FPS / DISP_MIN_FPS) {
                ebx_disp_drop_frame(&tick);
            }
#endif /*DISP_MIN_FPS*/
        }
        fps = ebx_disp_count_fps_cur();//(tick);
        if(fps > 0) {
            ESP_LOGI(TAG, "fps: %lu (%lu)", fps, cnt_draw);
            cnt_draw = 0;
            bool sram_dirty = gnuboy_sram_dirty();
            if(last_sram_dirty && sram_dirty) {
                ESP_LOGI(TAG, "save sram to: %s", g_sram_path);
                if(gnuboy_save_sram(g_sram_path, false)) {
                    ESP_LOGW(TAG, "save sram failed: %s", g_sram_path);
                }
                sram_dirty = gnuboy_sram_dirty();
            }
            last_sram_dirty = sram_dirty;
        }
        do_draw = (ebx_disp_wait_frame(&tick) >= 0);
    }
}

static void parse_menuconf(void) {
    g_menuconf_key = EBX_IPT_KEY_B;
    g_menuconf_keypress = 60;
    g_menuconf_pad_kpdef = 0;
    size_t slen = strlen(g_rom_path);
    if(slen > 0x400) {
        goto done;
    }
    int cidx = (int)slen - 1;
    int ced = -1;
    int hit_cnt = 0;
    for(; cidx >= 0; cidx--) {
        char c = g_rom_path[cidx];
        if(c == '.') {
            if(++hit_cnt == 1) {
                ced = cidx;
            } else if(hit_cnt == 2) {
                cidx++;
                break;
            }
        } else if(c == '/') {
            cidx = -1;
            break;
        }
    }
    if(cidx < 0 || cidx >= ced) {
        goto done;
    }
    switch(g_rom_path[cidx]) {
    case 'a':
        g_menuconf_key = EBX_IPT_KEY_A;
        cidx++;
        break;
    case 'b':
        cidx++;
        break;
    }
    if(cidx >= ced) {
        goto done;
    }
    int cva = -1;
    for(;;) {
        int cv = g_rom_path[cidx] - '0';
        if(cv >= 0 && cv <= 9) {
            if(cva < 0) cva = 0;
            cva = cva * 10 + cv;
            cidx++;
        } else {
            break;
        }
        if(cidx >= ced) {
            if(cva < 0) {
                goto done;
            } else {
                break;
            }
        }
    }
    if(cva >= 0) {
        g_menuconf_keypress = cva * 30;
    }
    if(cidx >= ced) {
        goto done;
    }
    if(g_rom_path[cidx] == 'e') {
        g_menuconf_pad_kpdef = 1;
    }
done:
    if(g_menuconf_key == EBX_IPT_KEY_A) {
        g_menuconf_pad = GB_PAD_A;
        ESP_LOGI(TAG, "menu conf / trigger key: A");
    } else {
        g_menuconf_pad = GB_PAD_B;
        ESP_LOGI(TAG, "menu conf / trigger key: B");
    }
    ESP_LOGI(TAG, "menu conf / trigger time: %d.%ds", g_menuconf_keypress / 60, g_menuconf_keypress % 60 / 6);
    if(g_menuconf_pad_kpdef) {
        g_menuconf_pad_kpdef = g_menuconf_pad;
        ESP_LOGI(TAG, "menu conf / emit key");
    }
}

REG_APP {
    ESP_LOGI(TAG, "start app gnuboy");
    g_rom_path = params[0];
    g_sram_path = params[1];
    g_stat_path = params[2];
    parse_menuconf();
    TaskHandle_t hndl_disp = NULL;
    xTaskCreate(app_task, "ebx_app_gnuboy", 0x4000, NULL, 3, &hndl_disp);
}
