#ifndef __INC_GNUBOY_CART_EXMEM_H__
#define __INC_GNUBOY_CART_EXMEM_H__

#include "gnuboy/hw.h"

#define CART_MAX_SZ         0x400000
#define CART_MAX_BLEN       ebx_exmem_sz2blen(CART_MAX_SZ)

#define CART_MAX_BANK       ( cart.romsize )

#define CART_BANK_SZ        0x4000
#define CART_BB_DIV         DIV_FLOOR(EBX_EXMEM_BLKSZ, CART_BANK_SZ)
#define CART_BANK2BIDX(b)   DIV_FLOOR(b, CART_BB_DIV)
#define CART_BIDX2BANK(b)   ( (b) * CART_BB_DIV )
#define CART_MAX_BANK_BIDX  CART_BANK2BIDX(CART_MAX_BANK)
#define CART_MAX_LOAD_BIDX  15

static ebx_exmem_hndl_t g_exmem = NULL;
static ebx_exmem_ctx_t  g_ctx = NULL;
static void*            g_ctx_base = NULL;

static size_t   g_rb_bidx_cur = 0;
static size_t   g_rb_bidx_min = 0;
static size_t   g_rb_cnt = 0;

static void cart_load_rombank(int bank) {
    if(cart.rombanks[bank]) return;
    const size_t bidx = CART_BANK2BIDX(bank) % CART_MAX_BANK_BIDX;
    size_t ctx_bidx = g_rb_cnt;
    if(ctx_bidx >= CART_MAX_LOAD_BIDX) {
        for(;;) {
            if(cart.rombanks[CART_BIDX2BANK(g_rb_bidx_cur)]) {
                ctx_bidx = ebx_exmem_buf2bidx(g_ctx_base, cart.rombanks[CART_BIDX2BANK(g_rb_bidx_cur)]);
                ebx_exmem_unmap(g_ctx, cart.rombanks[CART_BIDX2BANK(g_rb_bidx_cur)], 1);
                for(int i = 0; i < CART_BB_DIV; i++) {
                    DBG_LOGI(TAG, "release %zu rombank: %zu", ctx_bidx, CART_BIDX2BANK(g_rb_bidx_cur) + i);
                    cart.rombanks[CART_BIDX2BANK(g_rb_bidx_cur) + i] = NULL;
                }
                break;
            }
            if(++g_rb_bidx_cur >= CART_MAX_BANK_BIDX) {
                g_rb_bidx_cur = g_rb_bidx_min;
            }
        }
    } else {
        g_rb_cnt++;
    }
    void* buf = ebx_exmem_map(g_exmem, g_ctx, bidx, ctx_bidx, 1);
    for(int i = 0; i < CART_BB_DIV; i++) {
        DBG_LOGI(TAG, "load %zu rombank: %zu", ctx_bidx, CART_BIDX2BANK(bidx) + i);
        cart.rombanks[CART_BIDX2BANK(bidx) + i] = buf + i * CART_BANK_SZ;
    }
}

static inline void cart_inc_rb_min(int bank) {
    const size_t bidx = CART_BANK2BIDX(bank) + 1;
    if(bidx > g_rb_bidx_min) {
        g_rb_bidx_min = bidx;
    }
    if(bidx > g_rb_bidx_cur) {
        g_rb_bidx_cur = bidx;
    }
}

static inline void cart_write(size_t ofs, void* buf, size_t size) {
    ebx_exmem_write(g_exmem, ofs, size, buf);
}

static inline void cart_init(void) {
    g_exmem = ebx_exmem_alloc(CART_MAX_BLEN);
    cart.cb_load_rombank = &cart_load_rombank;
}

static inline void cart_post_init(void) {
    g_ctx = ebx_exmem_alloc_ctx(g_exmem, CART_MAX_LOAD_BIDX);
    g_ctx_base = ebx_exmem_get_ctx_base(g_exmem, g_ctx);
}

static inline void cart_deinit(void) {
    ebx_exmem_free_ctx(g_ctx);
    g_ctx = NULL;
    ebx_exmem_free(g_exmem);
    g_exmem = NULL;
}

static inline void cart_load_rom_from_exmem(void) {
    byte buf[0x200];
    ebx_exmem_read(g_exmem, 0, 0x200, buf);
    if(gnuboy_load_rom(buf, 0x200)) {
        ESP_LOGE(TAG, "rom setup failed");
        abort();
    }
    cart_load_rombank(0);
    cart_inc_rb_min(0);
}

#define CART_LOAD_BLKSZ     0x1000
static inline void cart_load_rom_from_file(const char* fn) {
    ESP_LOGI(TAG, "loading rom file: %s", fn);
    FILE* fp = fopen(fn, "rb");
    if(!fp) {
        ESP_LOGE(TAG, "rom open failed: %s", fn);
        abort();
    }
    cart_init();
    byte buf[CART_LOAD_BLKSZ];
    size_t ofs = 0;
    size_t rdlen;
    while( (rdlen = fread(buf, 1, CART_LOAD_BLKSZ, fp)) > 0 ) {
        ebx_exmem_write(g_exmem, ofs, rdlen, buf);
        ofs += rdlen;
    }
    fclose(fp);
    ESP_LOGI(TAG, "write done: 0x%zx", ofs);
    cart_post_init();
    cart_load_rom_from_exmem();
}

static size_t write_zip_to_exmem(void* buf, size_t sz, void* ctx) {
    size_t* p_ofs = ctx;
    ebx_exmem_write(g_exmem, *p_ofs, sz, buf);
    *p_ofs += sz;
    return sz;
}

#define CART_LOAD_UNZIP_SZ_I    32 * 0x400
#define CART_LOAD_UNZIP_SZ_O    32 * 0x400
static inline void cart_load_rom_from_zip_file(const char* fn) {
    ESP_LOGI(TAG, "loading rom zip file: %s", fn);
    FILE* fp = fopen(fn, "rb");
    if(!fp) {
        ESP_LOGE(TAG, "rom open failed: %s", fn);
        abort();
    }
    cart_init();
    size_t ofs = 0;
    if(ebx_zip_unzip_file(CART_LOAD_UNZIP_SZ_I, CART_LOAD_UNZIP_SZ_O, fp, write_zip_to_exmem, &ofs)) {
        ESP_LOGE(TAG, "unzip failed: %s", fn);
        abort();
    } else {
        ESP_LOGI(TAG, "unzip done");
    }
    fclose(fp);
    ESP_LOGI(TAG, "write done: 0x%zx", ofs);
    cart_post_init();
    cart_load_rom_from_exmem();
}

#else
#error duplicated include
#endif /*__INC_GNUBOY_CART_EXMEM_H__*/
