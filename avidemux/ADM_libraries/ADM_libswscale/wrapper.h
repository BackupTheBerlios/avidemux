/*

	Replacement for assert
*/
#include "ADM_assert.h"
//******************
#undef malloc
#undef realloc
#undef memalign
#undef free
#undef strdup
//******************
#define malloc ADM_alloc
#define realloc ADM_realloc
#define memalign ADM_memalign
#define free  ADM_dealloc
#define strdup ADM_strdup


