#ifndef __INC_EBXP_APP_H__
#define __INC_EBXP_APP_H__

#define _APP_INIT_METH(n)   ebx_app_##n##_init
#define DECL_APP(n)         void _APP_INIT_METH(n)(void)
#define INIT_APP(n)         _APP_INIT_METH(n)()
#define REG_APP             DECL_APP(APP_NAME)

#endif /*__INC_EBXP_APP_H__*/
