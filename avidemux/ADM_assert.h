/*
	Replacement for assert etc...
*/
#ifndef ADM_ASSERT_H
#define ADM_ASSERT_H

#include <assert.h>
#define ADM_assert(x) { if(!(x)) {assert(0);printf("Fatal error :"__FILE__"\n");  }}

/* Functions we want to override to have better os support / debug / error control */
    
#ifdef __cplusplus
extern "C" {
#endif
size_t ADM_fread (void *ptr, size_t size, size_t n, FILE *sstream);
size_t ADM_fwrite (void *ptr, size_t size, size_t n, FILE *sstream);
FILE  *ADM_fopen (const char *file, const char *mode);
int    ADM_fclose (FILE *file);

extern void *ADM_alloc(size_t size);
extern void *ADM_calloc(size_t nbElm,size_t elSize);
extern void *ADM_realloc(void *in,size_t size);
extern void ADM_dezalloc(void *ptr);
extern char *ADM_strdup( const char *in);
extern char *slashToBackSlash(char *in);
typedef void *(* adm_fast_memcpy)(void *to, const void *from, size_t len);
extern adm_fast_memcpy myAdmMemcpy;

#define ADM_memalign(x,y) ADM_alloc(y)

#define ADM_dealloc(x) ADM_dezalloc( (void *)x)
#define memcpy myAdmMemcpy

// Override fread/fwrite ..
#define fread   ADM_fread
#define fwrite  ADM_fwrite
#define fopen   ADM_fopen
#define fclose  ADM_fclose

#ifndef ADM_LEGACY_PROGGY
  #define malloc #error
  #define realloc #error
  #define memalign #error
  #define free  #error
  #define strdup #error
  #define calloc #error
#else
  #define malloc ADM_alloc
  #define realloc ADM_realloc
  #define memalign(x,y) ADM_alloc(y)
  #define free  ADM_dealloc
  #define strdup ADM_strdup
  #define calloc ADM_calloc
#endif
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
