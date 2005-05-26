/*


*/
#ifndef DMX_IDENT
#define DMX_IDENT

typedef enum
{
        DMX_MPG_UNKNOWN=0,
        DMX_MPG_ES,
        DMX_MPG_PS,
        DMX_MPG_TS
}DMX_TYPE;
DMX_TYPE dmxIdentify(char *name);
#endif
