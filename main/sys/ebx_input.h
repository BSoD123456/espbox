#ifndef __INC_EBX_INPUT_H__
#define __INC_EBX_INPUT_H__

enum {
    EBX_IPT_KEY_UP = 0,
    EBX_IPT_KEY_DOWN,
    EBX_IPT_KEY_LEFT,
    EBX_IPT_KEY_RIGHT,
    EBX_IPT_KEY_A,
    EBX_IPT_KEY_B,
    EBX_IPT_NUM_KEYS,
};

#define EBX_IPT_CHK_KEYS(k, v)  ((k) & (1 << (v)))

void ebx_ipt_init(void);
uint32_t ebx_ipt_get(void);

#endif /*__INC_EBX_INPUT_H__*/
