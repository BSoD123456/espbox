#ifndef __INC_EBX_COMMON_H__
#define __INC_EBX_COMMON_H__

#define DIV_CEIL(v, a)      ( ((size_t)(v) + (size_t)(a) - 1) / (size_t)(a) )
#define DIV_FLOOR(v, a)     ( (size_t)(v) / (size_t)(a) )

#define ALIGN_UP(v, a)      ( DIV_CEIL(v, a) * (size_t)(a) )
#define ALIGN_DOWN(v, a)    ( DIV_FLOOR(v, a) * (size_t)(a) )
#define ALIGN_CHK(v, a)     ( (size_t)(v) % (size_t)(a) == 0)

#define FLAG_MATCH(v, f)    ( ((v) & (f)) == (f) )

#define MAX(a, b)           (((a) > (b)) ? (a) : (b))
#define MIN(a, b)           (((a) < (b)) ? (a) : (b))

#define DIVMOD(a, b)        (((a) < 0) ? ((a) % (b) + (b)) : ((a) % (b)))

#endif /*__INC_EBX_COMMON_H__*/
