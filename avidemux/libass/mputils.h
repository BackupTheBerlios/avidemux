#ifndef __MPUTILS_H__
#define __MPUTILS_H__

unsigned utf8_get_char(char **str);

void my_mp_msg(int lvl, char *lvl_str, char *fmt, ...);

#define mp_msg(mod, level, args...) my_mp_msg(level, #level, args)

#define MSGT_GLOBAL 0
#define MSGT_SUBREADER 36

#define MSGL_FATAL 0
#define MSGL_ERR 1
#define MSGL_WARN 2
#define MSGL_INFO 4
#define MSGL_V 6
#define MSGL_DBG2 7

#endif
