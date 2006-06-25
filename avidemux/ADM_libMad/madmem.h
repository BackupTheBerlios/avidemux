#ifndef MADMEM_H
#define MADMEM_H
extern void *ADM_alloc(int size);
extern void ADM_dezalloc(void *ptr);
#define malloc ADM_alloc
#define free ADM_dezalloc
#endif

