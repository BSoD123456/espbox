#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"
#include "miniz.h"

#include "ebx_zip.h"

//static const char* TAG = "ebx_zip";

void ebx_zip_unzip_file(FILE* fp_in, ebx_zip_cb_t cb_out, size_t sz_ibuf, size_t sz_obuf) {
    tinfl_decompressor inflator;
    tinfl_init(&inflator);
}

