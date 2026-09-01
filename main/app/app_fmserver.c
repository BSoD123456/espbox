#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "ebx_sys.h"
#include "app.h"

#define APP_NAME fmserver
//static const char* TAG = "ebx_app_fmserver";

esp_err_t example_start_file_server(const char *base_path);

REG_APP {
    printf(params[0]);
    example_start_file_server("/storage");
}
