#ifndef __INC_GNUBOY_CART_EXMEM_H__
#define __INC_GNUBOY_CART_EXMEM_H__

#include "gnuboy/hw.h"

#define CART_MAX_SZ         0x400000
#define CART_MAX_BLEN       ebx_exmem_sz2blen(CART_MAX_SZ)

#define CART_MAX_BANK       0x100
#define CART_MAX_LOAD       0x20

#define CART_BANK_SZ        0x4000
#define CART_BB_DIV         DIV_FLOOR(EBX_EXMEM_BLKSZ, CART_BANK_SZ)
#define CART_MAX_BANK_BIDX  (CART_MAX_BANK / CART_BB_DIV)
#define CART_MAX_LOAD_BIDX  (CART_MAX_LOAD / CART_BB_DIV)

static ebx_exmem_hndl_t g_exmem = NULL;
static ebx_exmem_ctx_t  g_ctx = NULL;

static size_t   g_ring_bidx_cur = 0;
static size_t   g_ring_bidx_min = 0;
static size_t   g_ring_cnt = 0;

static void cart_load_rombank(int bank) {
    if(cart.rombanks[bank]) return;
    const size_t bidx = ALIGN_DOWN(bank, CART_BB_DIV) % CART_MAX_BANK_BIDX;
    while(g_ring_cnt >= CART_MAX_BLEN) {
    }
    void* buf = ebx_exmem_map(g_exmem, g_ctx, bidx, g_ring_bidx_cur, 1);
}

static inline void cart_write(size_t ofs, void* buf, size_t size) {
    ebx_exmem_write(g_exmem, ofs, size, buf);
}

static inline void cart_init() {
    g_exmem = ebx_exmem_alloc(CART_MAX_BLEN);
    cart.cb_load_rombank = &cart_load_rombank;
}

static inline void cart_post_init() {
    g_ctx = ebx_exmem_alloc_ctx(g_exmem, CART_MAX_LOAD_BIDX);
}

static inline void cart_deinit() {
    ebx_exmem_free_ctx(g_ctx);
    g_ctx = NULL;
    ebx_exmem_free(g_exmem);
    g_exmem = NULL;
}

#else
#error duplicated include
#endif /*__INC_GNUBOY_CART_EXMEM_H__*/
