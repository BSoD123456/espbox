#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"

#include "ebx_exmem.h"

struct ebx_exmem_stream_s {
    ebx_exmem_ctx_t ctx;
    size_t cur_blk_idx;
    void* cur_blk_buf;
};

void ebx_exmem_alloc_stream(ebx_exmem_hndl_t mh) {
    ebx_exmem_stream_t stm = malloc(sizeof(struct ebx_exmem_stream_s));
    stm->ctx = ebx_exmem_alloc_ctx(mh);
    stm->cur_blk_idx = 0;
    stm->cur_blk_buf = NULL;
    return stm;
}

void ebx_exmem_write(ebx_exmem_hndl_t mh, void* buf, size_t len) {
    
}
