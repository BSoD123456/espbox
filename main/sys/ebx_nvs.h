#ifndef __INC_EBX_NVS_H__
#define __INC_EBX_NVS_H__

void ebx_nvs_init(void);
void ebx_nvs_deinit(void);

#define _EBX_NVS_ACESS_PROTO(tn, tt)    \
tt ebx_nvs_get_##tn(const char* key);   \
void ebx_nvs_set_##tn(const char* key, tt val)

_EBX_NVS_ACESS_PROTO(i8, int8_t);
_EBX_NVS_ACESS_PROTO(u8, uint8_t);
_EBX_NVS_ACESS_PROTO(i32, int32_t);
_EBX_NVS_ACESS_PROTO(u32, uint32_t);

#endif /*__INC_EBX_NVS_H__*/
