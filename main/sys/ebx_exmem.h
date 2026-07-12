#ifndef __INC_EBX_EXMEM_H__
#define __INC_EBX_EXMEM_H__

#include "esp32/himem.h"
#include "esp_err.h"

typedef esp_himem_handle_t ebx_exmem_hndl_t;
typedef esp_himem_rangehandle_t ebx_exmem_ctx_t;

typedef struct ebx_exmem_stream_s ebx_exmem_stream_t;

inline ebx_exmem_hndl_t ebx_exmem_alloc(size_t size) {
    ebx_exmem_hndl_t mh;
    ESP_ERROR_CHECK(esp_himem_alloc(size, &mh));
    return mh;
}

inline void ebx_exmem_free(ebx_exmem_hndl_t mh) {
    ESP_ERROR_CHECK(esp_himem_free(mh));
}

inline ebx_exmem_ctx_t ebx_exmem_alloc_ctx(ebx_exmem_hndl_t mh) {
    ebx_exmem_ctx_t ctx;
    ESP_ERROR_CHECK(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &ctx));
    return ctx
}

inline void ebx_exmem_free_ctx(ebx_exmem_ctx_t ctx) {
    ESP_ERROR_CHECK(esp_himem_free_map_range(ctx));
}

#endif /*__INC_EBX_EXMEM_H__*/
