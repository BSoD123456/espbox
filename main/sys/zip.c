#include <stdio.h>

#define DBG_HOST
//#undef DBG_HOST

#ifdef DBG_HOST

#define LOGI(m, ...)    printf("I: " m "\n", ##__VA_ARGS__)
#define LOGW(m, ...)    printf("W: " m "\n", ##__VA_ARGS__)
#define LOGE(m, ...)    printf("E: " m "\n", ##__VA_ARGS__)

#else

#include "esp_err.h"
#include "esp_log.h"

static const char* TAG = "ebx_zip";

#define LOGI(...)   ESP_LOGI(TAG, ##__VA_ARGS__)
#define LOGW(...)   ESP_LOGW(TAG, ##__VA_ARGS__)
#define LOGE(...)   ESP_LOGE(TAG, ##__VA_ARGS__)

#endif

#include "miniz.h"

#include "ebx_zip.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define OK      0
#define ERR     1

static int decompress_file(size_t sz_all, size_t sz_ibuf, size_t sz_obuf, FILE* fp_in, ebx_zip_cb_t cb_out, void* ctx_out) {
    int ret = ERR;

    if(sz_obuf < TINFL_LZ_DICT_SIZE) {
        LOGE("obuf too small");
        goto done0;
    }
    size_t rm_all = sz_all;

    void* ibuf = malloc(sz_ibuf);
    if(!ibuf) {
        LOGE("no mem");
        goto done0;
    }
    void* obuf = malloc(sz_obuf);
    if(!obuf) {
        LOGE("no mem");
        goto done1;
    }

    tinfl_decompressor inflator;
    tinfl_init(&inflator);

    size_t rm_ibuf = 0;
    size_t rm_obuf = sz_obuf;
    void* inxt = ibuf;
    void* onxt = obuf;

    for(;;) {
        if(rm_ibuf == 0) {
            if(rm_all == 0) {
                LOGE("need more input");
                goto done2;
            }
            rm_ibuf = MIN(sz_ibuf, rm_all);
            if(fread(ibuf, 1, rm_ibuf, fp_in) != rm_ibuf) {
                LOGE("read input failed");
                goto done2;
            }
            rm_all -= rm_ibuf;
            inxt = ibuf;
        }

        size_t ilen = rm_ibuf;
        size_t olen = rm_obuf;
        tinfl_status status = tinfl_decompress(&inflator, inxt, &ilen, obuf, onxt, &olen, rm_all ? TINFL_FLAG_HAS_MORE_INPUT : 0);
        rm_ibuf -= ilen;
        rm_obuf -= olen;
        inxt = (void*)((uint8_t*)inxt + ilen);
        onxt = (void*)((uint8_t*)onxt + olen);
        
        if(status <= TINFL_STATUS_DONE || !rm_obuf) {
            size_t rlen = sz_obuf - rm_obuf;
            if(rlen > 0) {
                if(cb_out(obuf, rlen, ctx_out) != rlen) {
                    LOGE("output failed");
                    goto done2;
                }
                onxt = obuf;
                rm_obuf = sz_obuf;
            }
        }
        
        if(status == TINFL_STATUS_DONE) {
            break;
        } else if(status < TINFL_STATUS_DONE) {
            LOGE("decompress failed: %i", status);
            goto done2;
        }
    }
    
    ret = OK;
done2:
    free(obuf);
done1:
    free(ibuf);
done0:
    return ret;
}


static size_t parse_zip_header(FILE* fp_in) {
#define ZIPHD_SZ    30
    uint8_t header[ZIPHD_SZ];
#define ZIPHD(i, t) (*(uint##t##_t*)(&(header[i])))
    if(fread(header, 1, ZIPHD_SZ, fp_in) != ZIPHD_SZ) {
        LOGE("invalid zip header");
        return 0;
    }
    if(ZIPHD(0, 32) != 0x04034b50) {
        LOGE("not zip header");
        return 0;
    }
    if( (header[8] | (header[9] << 8)) != 8 ) {
        LOGE("not deflate");
        return 0;
    }
    if(fseek(fp_in, ZIPHD(26, 16) + ZIPHD(28, 16), SEEK_CUR)) {
        LOGE("seek data failed");
        return 0;
    }
    return (size_t)ZIPHD(18, 32);
}

int ebx_zip_unzip_file(size_t sz_ibuf, size_t sz_obuf, FILE* fp_in, ebx_zip_cb_t cb_out, void* ctx_out) {
    size_t sz_all = parse_zip_header(fp_in);
    if(sz_all == 0) {
        return ERR;
    }
    return decompress_file(sz_all, sz_ibuf, sz_obuf, fp_in, cb_out, ctx_out);
}
