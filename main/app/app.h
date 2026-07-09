#ifndef __INC_EBXP_APP_H__
#define __INC_EBXP_APP_H__

#define _APP_INIT_METH(n)   ebx_app_##n##_init
#define DECL_APP(n)         void _APP_INIT_METH(n)(void** params)
#define REG_APP             DECL_APP(APP_NAME)

#define SET_APP_PARAM(n)    void* __ebx_app_##n##_param[]=
#define INIT_APP(n)         _APP_INIT_METH(n)(__ebx_app_##n##_param)

#endif /*__INC_EBXP_APP_H__*/
