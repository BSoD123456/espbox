#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "ebx_exmem.h"

#define _UPKA(...)                              __VA_ARGS__
#define MAKE_EXMEM_ACCESS(name, args, opcode)   \
void ebx_exmem_##name(ebx_exmem_hndl_t mh, size_t ofs, size_t size, _UPKA args) {   \
    size_t bidx = ebx_exmem_ofs2bidx(ofs);   \
    size_t aofs = ebx_exmem_blk2sz(bidx);   \
    size_t bofs = ofs - aofs;   \
    size_t blen = ebx_exmem_sz2blen(size + bofs);   \
    ebx_exmem_ctx_t ctx = ebx_exmem_alloc_ctx(mh, blen);    \
    void* ext_buf = ebx_exmem_map(mh, ctx, bidx, 0, blen);  \
    opcode;     \
    ebx_exmem_unmap(ctx, ext_buf, blen);    \
    ebx_exmem_free_ctx(ctx);    \
}

MAKE_EXMEM_ACCESS(read, (void* buf), {
    memcpy(buf, ext_buf + bofs, size);
});

MAKE_EXMEM_ACCESS(write, (void* buf), {
    memcpy(ext_buf + bofs, buf, size);
});
