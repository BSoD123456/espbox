#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"

#include "esp_littlefs.h"

#include "ebx_fs.h"

static const char* TAG = "ebx_fs";

void ebx_fs_init(void) {
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/storage",
        .partition_label = NULL,
        .format_if_mount_failed = false
    };
    ESP_ERROR_CHECK(esp_vfs_littlefs_register(&conf));
}
