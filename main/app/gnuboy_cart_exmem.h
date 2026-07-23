#ifndef __INC_GNUBOY_CART_EXMEM_H__
#define __INC_GNUBOY_CART_EXMEM_H__

#include "gnuboy/hw.h"

#define CART_MAX_SZ     0x400000
#define CART_MAX_BLEN   ebx_exmem_sz2blen(CART_MAX_SZ)

static ebx_exmem_hndl_t g_exmem = NULL;

static void cart_load_rombank(int bank) {
}

static inline void cart_write(size_t ofs, void* buf, size_t size) {
    ebx_exmem_write(g_exmem, ofs, size, buf);
}

static inline void cart_init() {
    g_exmem = ebx_exmem_alloc(CART_MAX_BLEN);
    cart.cb_load_rombank = &cart_load_rombank;
}

#else
#error duplicated include
#endif /*__INC_GNUBOY_CART_EXMEM_H__*/
