/*

	Replacement for assert
*/
#ifndef ADM_ASSERT_H
#define ADM_ASSERT_H

#include <assert.h>
#define ADM_assert(x) { if(!(x)) {assert(0);printf("Fatal error :"__FILE__"\n");  }}

#ifdef __cplusplus
extern "C" {
#endif
extern void *ADM_alloc(size_t size);
extern void *ADM_realloc(void *in,size_t size);
extern void ADM_dezalloc(void *ptr);
extern char *ADM_strdup( const char *in);
extern char *slashToBackSlash(char *in);
typedef void *(* adm_fast_memcpy)(void *to, const void *from, size_t len);
extern adm_fast_memcpy myAdmMemcpy;

#define ADM_memalign(x,y) ADM_alloc(y)

#define ADM_dealloc(x) ADM_dezalloc( (void *)x)
#define memcpy myAdmMemcpy

#define malloc #error
#define realloc #error
#define memalign #error
#define free  #error
#define strdup #error

#ifdef __cplusplus
}
#endif
// Compatibility with fprintf etc.. with long long & win32
#ifdef CYG_MANGLING
        #define LLX "I64x"
        #define LLU "I64u"
        
        #define cleanupPath(x) slashToBackSlash(x)
#else
        #define LLX "llx"
        #define LLU "llu"
        #define cleanupPath(x) ADM_strdup(x)
#endif

#endif
