#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"
#include "miniz.h"

#include "ebx_zip.h"

static const char* TAG = "ebx_zip";

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void ebx_zip_unzip_file(FILE* fp_in, ebx_zip_cb_t cb_out, size_t sz_ibuf, size_t sz_obuf) {
    fseek(fp_in, 0, SEEK_END);
    long floc = ftell(fp_in);
    fseek(fp_in, 0, SEEK_SET);
    if(floc < 0) {
        ESP_LOGE(TAG, "invalid file");
        abort();
    }
    size_t rm_in = floc;

    void* ibuf = malloc(sz_ibuf);
    void* obuf = malloc(sz_obuf);

    tinfl_decompressor inflator;
    tinfl_init(&inflator);

    for(;;) {
        size_t ilen = fread(ibuf, 1, sz_ibuf, fp_in);
        if(ilen == 0) {
            // TODO
        }
        size_t olen = sz_obuf;
        tdefl_status status = tinfl_decompress(&inflator, ibuf, &ilen, obuf, obuf, &olen, (infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0) | TINFL_FLAG_PARSE_ZLIB_HEADER);

    }
}

