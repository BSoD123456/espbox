#include <stdio.h>
#include <sys/errno.h>

#include "freertos/FreeRTOS.h"
#include "esp_vfs.h"
#include "esp_err.h"
#include "esp_log.h"

#include "ebx_himemdisk.h"

static const char* TAG = "ebx_hmdsk";

static const esp_vfs_t hmdsk_drv = {
    .flags      = ESP_VFS_FLAG_DEFAULT,
    .open       = &hmdsk_open,
    .fstat      = &hmdsk_fstat,
    .close      = &hmdsk_close,
    .write      = &hmdsk_write,
    .read       = &hmdsk_read,
    .lseek      = &hmdsk_lseek,
};

void ebx_hmdsk_init(const char* devname) {
    ESP_ERROR_CHECK(esp_vfs_register("/" devname, &hmdsk_drv, NULL));
}
