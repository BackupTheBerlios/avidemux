/*

	Replacement for assert
*/
#include <assert.h>
#define ADM_assert(x) { if(!(x)) {assert(0);printf("Fatal error :"__FILE__"\n");  }}
