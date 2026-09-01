#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"

#include "ebx_sys.h"
#include "ebx_app.h"

static char* TAG = "ebx_main";

#define MAX_INNER_ENTRIES   1

#define APP_SURFIX_0        ".gbc.zip"

const char* g_root = "/storage";

static const char* find_surfix(const char* dst, const char* sub) {
    size_t dlen = strlen(dst);
    size_t slen = strlen(sub);
    if(dlen < slen) {
        return NULL;
    }
    const char* rsub = dst + dlen - slen;
    if(memcmp(rsub, sub, slen) == 0) {
        return rsub;
    } else {
        return NULL;
    }
}

static size_t find_noext(const char* dst, const char* ext) {
    const char* sub = strrchr(dst, '/');
    if(sub == NULL) sub = dst;
    sub = find_surfix(sub, ext);
    if(sub == NULL) {
        return strlen(dst);
    } else {
        return sub - dst;
    }
}

static int match_file(int didx, const char* surfix, char** p_rname) {
    DIR* root = opendir(g_root);
    if(!root) {
        ESP_LOGE(TAG, "open root failed");
        abort();
    }
    struct dirent* ent;
    int mcnt = 0;
    char* rname = NULL;
    while( (ent = readdir(root)) != NULL ) {
        if(ent->d_type != DT_REG) continue;
        const char* sfsub = find_surfix(ent->d_name, surfix);
        if(sfsub == NULL || mcnt++ < didx) continue;
        size_t rlen = strlen(g_root) + strlen(ent->d_name) + 2;
        rname = malloc(rlen);
        snprintf(rname, rlen, "%s/%s", g_root, ent->d_name);
        break;
    }
    closedir(root);
    *p_rname = rname;
    assert(didx - mcnt >= -1);
    return didx - mcnt;
}

void run_gnuboy(const char* rom_name, const char* sav_name, const char* sta_name) {
    ebx_disp_init();
    ebx_ipt_init();
    SET_APP_PARAM(gnuboy) {
        (void*)rom_name,
        (void*)sav_name,
        (void*)sta_name,
        NULL
    };
    INIT_APP(gnuboy);
}

void app_main(void) {
    ebx_nvs_init();
    ebx_fs_init();
    uint8_t pwflags = ebx_nvs_get_u8("power_flags");
    int stidx = (int)ebx_nvs_get_u8("power_start_idx");
    ESP_LOGI(TAG, "power on: flags=0x%x, start_idx=%d", pwflags, stidx);
    int ostidx = stidx;
    if(pwflags == 1) {
        stidx++;
    } else {
        ebx_nvs_set_u8("power_flags", 1);
    }
    char* dfname;
    int cstidx = stidx;
    int phase;
    for(;;) {
        phase = 0;
        cstidx = match_file(cstidx, APP_SURFIX_0, &dfname);
        if(cstidx < 0) break;
        assert(dfname == NULL);
        phase++;
        cstidx -= MAX_INNER_ENTRIES;
        if(cstidx < 0) break;
        stidx = cstidx;
    }
    if(stidx != ostidx) {
        if(stidx > 0xff || stidx < 0) {
            ESP_LOGE(TAG, "invalid power_start_idx");
            abort();
        }
        ebx_nvs_set_u8("power_start_idx", (uint8_t)stidx);
    }
    switch(phase) {
    case 0:
        assert(dfname != NULL);
        char* rom_name = strdup(dfname);
        ESP_LOGI(TAG, "enter file entry: %s", rom_name);
        size_t blen = find_noext(rom_name, APP_SURFIX_0);
        size_t dlen = blen + 5;
        char* sav_name = malloc(dlen);
        memcpy(sav_name, rom_name, blen);
        memcpy(sav_name + blen, ".sav\0", 5);
        ESP_LOGI(TAG, "sav file: %s", sav_name);
        char* sta_name = malloc(dlen);
        memcpy(sta_name, rom_name, blen);
        memcpy(sta_name + blen, ".sta\0", 5);
        ESP_LOGI(TAG, "sta file: %s", sta_name);
        run_gnuboy(rom_name, sav_name, sta_name);
        /* should not free path name */
        /*free(sav_name);
        free(sta_name);
        free(rom_name);*/
        break;
    case 1:
        ESP_LOGI(TAG, "enter inner entry: %d", cstidx + MAX_INNER_ENTRIES);
        switch(cstidx + MAX_INNER_ENTRIES) {
        case 0:
            ebx_disp_init();
            ebx_wifi_init();
            SET_APP_PARAM(fmserver) {
                NULL
            };
            INIT_APP(fmserver);
            break;
        }
        break;
    }
    if(dfname != NULL) {
        free(dfname);
    }
}
