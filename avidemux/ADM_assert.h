/*

	Replacement for assert
*/
#include <assert.h>
#define ADM_assert(x) { if(!(x)) {assert(0);printf("Fatal error :"__FILE__"\n");  }}

#ifdef __cplusplus
extern "C" {
#endif
extern void *ADM_alloc(size_t size);
extern void ADM_dezalloc(void *ptr);

#define ADM_dealloc(x) ADM_dezalloc( (void *)x)

#ifdef __cplusplus
}
#endif

