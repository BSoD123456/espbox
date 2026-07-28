#ifndef __INC_EBX_ZIP_H__
#define __INC_EBX_ZIP_H__

typedef size_t(*ebx_zip_cb_t)(void* buf, size_t sz, void* ctx);

int ebx_zip_unzip_file(size_t sz_ibuf, size_t sz_obuf, FILE* fp_in, ebx_zip_cb_t cb_out, void* ctx_out);

#endif /*__INC_EBX_ZIP_H__*/
