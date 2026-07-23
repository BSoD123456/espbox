#ifndef __INC_EBX_EXMEM_H__
#define __INC_EBX_EXMEM_H__

#include "esp32/himem.h"
#include "esp_err.h"

#define EBX_EXMEM_BLKSZ     ESP_HIMEM_BLKSZ

#define DIV_CEIL(v, a)      ( ((size_t)(v) + (size_t)(a) - 1) / (size_t)(a) )
#define DIV_FLOOR(v, a)     ( (size_t)(v) / (size_t)(a) )

#define ALIGN_UP(v, a)      ( DIV_CEIL(v, a) * (size_t)(a) )
#define ALIGN_DOWN(v, a)    ( DIV_FLOOR(v, a) * (size_t)(a) )
#define ALIGN_CHK(v, a)     ( (size_t)(v) % (size_t)(a) == 0)

#define BLKSZ(sz)           ALIGN_UP(sz, EBX_EXMEM_BLKSZ)
#define BLKOFS(ofs)         ALIGN_DOWN(ofs, EBX_EXMEM_BLKSZ)

typedef esp_himem_handle_t ebx_exmem_hndl_t;
typedef esp_himem_rangehandle_t ebx_exmem_ctx_t;

static inline size_t ebx_exmem_sz2blen(size_t sz) {
    return DIV_CEIL(sz, EBX_EXMEM_BLKSZ);
}

static inline size_t ebx_exmem_ofs2bidx(size_t ofs) {
    return DIV_FLOOR(ofs, EBX_EXMEM_BLKSZ);
}

static inline size_t ebx_exmem_blk2sz(size_t blk) {
    return blk * EBX_EXMEM_BLKSZ;
}

static inline ebx_exmem_hndl_t ebx_exmem_alloc(size_t blen) {
    ebx_exmem_hndl_t mh;
    ESP_ERROR_CHECK(esp_himem_alloc(ebx_exmem_blk2sz(blen), &mh));
    return mh;
}

static inline void ebx_exmem_free(ebx_exmem_hndl_t mh) {
    ESP_ERROR_CHECK(esp_himem_free(mh));
}

static inline ebx_exmem_ctx_t ebx_exmem_alloc_ctx(ebx_exmem_hndl_t mh, size_t blen) {
    ebx_exmem_ctx_t ctx;
    ESP_ERROR_CHECK(esp_himem_alloc_map_range(ebx_exmem_blk2sz(blen), &ctx));
    return ctx
}

static inline void ebx_exmem_free_ctx(ebx_exmem_ctx_t ctx) {
    ESP_ERROR_CHECK(esp_himem_free_map_range(ctx));
}

static inline void* ebx_exmem_map(ebx_exmem_hndl_t mh, ebx_exmem_ctx_t ctx, size_t ext_bidx, size_t ctx_bidx, size_t blen) {
    void* buf;
    ESP_ERROR_CHECK(esp_himem_map(mh, ctx, ebx_exmem_blk2sz(ext_bidx), ebx_exmem_blk2sz(ctx_bidx), ebx_exmem_blk2sz(blen), 0, &buf));
    return buf;
}

static inline void ebx_exmem_unmap(ebx_exmem_ctx_t ctx, void* buf, size_t blen) {
    ESP_ERROR_CHECK(esp_himem_unmap(ctx, buf, ebx_exmem_blk2sz(blen)));
}

#endif /*__INC_EBX_EXMEM_H__*/
