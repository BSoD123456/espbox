#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"

#include "ebx_sys.h"
#include "ebx_app.h"

static char* TAG = "ebx_main";

#define MAX_INNER_ENTRIES   1

const char* g_root = "/storage";

static int find_surfix(const char* dst, const char* sub) {
    size_t dlen = strlen(dst);
    size_t slen = strlen(sub);
    int didx = dlen - slen;
    if(didx < 0) {
        return -1;
    }
    if(memcmp(dst + didx, sub, slen) == 0) {
        return didx;
    } else {
        return -1;
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
        int sfidx = find_surfix(ent->d_name, surfix);
        if(sfidx < 0 || mcnt++ < didx) continue;
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

void app_main(void) {
    ebx_nvs_init();
    ebx_fs_init();
    uint8_t pwflags = ebx_nvs_get_u8("power_flags");
    int stidx = (int)ebx_nvs_get_u8("power_start_idx");
    printf("power nvs: 0x%x, %d\n", pwflags, stidx);
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
        cstidx = match_file(cstidx, ".gbc.zip", &dfname);
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
        ESP_LOGI(TAG, "enter file entry: %s", dfname);
        free(dfname);
        break;
    case 1:
        ESP_LOGI(TAG, "enter inner entry: %d", cstidx + MAX_INNER_ENTRIES);
        break;
    }
    return;

    ebx_disp_init();
    ebx_ipt_init();
#if 1
    SET_APP_PARAM(gnuboy) {
        "/storage/siren2.a2.zip",
        "/storage/siren2.sav",
        "/storage/siren2.sta",
        NULL
    };
    INIT_APP(gnuboy);
#endif
}
