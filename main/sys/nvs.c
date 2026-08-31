#include <stdio.h>

#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"

#include "ebx_nvs.h"

//static const char* TAG = "ebx_nvs";

static bool g_nvs_inited = false;
static nvs_handle_t g_nvs_hndl;

#define ACESS_IMPL(tn, tt)                                      \
tt ebx_nvs_get_##tn(const char* key) {                          \
    tt val;                                                     \
    esp_err_t err = nvs_get_##tn(g_nvs_hndl, key, &val);        \
    if(err == ESP_ERR_NVS_NOT_FOUND) {                          \
        val = 0;                                                \
    } else {                                                    \
        ESP_ERROR_CHECK(err);                                   \
    }                                                           \
    return val;                                                 \
}                                                               \
void ebx_nvs_set_##tn(const char* key, tt val) {                \
    ESP_ERROR_CHECK(nvs_set_##tn(g_nvs_hndl, key, val));         \
}

ACESS_IMPL(i8, int8_t);
ACESS_IMPL(u8, uint8_t);
ACESS_IMPL(i32, int32_t);
ACESS_IMPL(u32, uint32_t);

void ebx_nvs_init(void) {
    if(g_nvs_inited) {
        return;
    }
    g_nvs_inited = true;
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(nvs_open("ebx_global", NVS_READWRITE, &g_nvs_hndl));
}

void ebx_nvs_deinit(void) {
    if(!g_nvs_inited) {
        return;
    }
    nvs_close(g_nvs_hndl);
    nvs_flash_deinit();
    g_nvs_inited = false;
}
