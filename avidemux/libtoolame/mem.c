#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toolame.h"
#include "toolame_global_flags.h"
#include "common.h"
#include "mem.h"

/*******************************************************************************
*
*  Allocate number of bytes of memory equal to "block".
*
*******************************************************************************/

void *toolame_malloc (unsigned long block, char *item)
{

  void *ptr;

  ptr = (void *) ADM_alloc (block);

  if (ptr != NULL) {
    memset (ptr, 0, block);
  } else {
    fprintf (stderr, "Unable to allocate %s\n", item);
    exit (0);
  }
  return (ptr);
}


/****************************************************************************
*
*  Free memory pointed to by "*ptr_addr".
*
*****************************************************************************/

void toolame_free (void **ptr_addr)
{

  if (*ptr_addr != NULL) {
    free (*ptr_addr);
    *ptr_addr = NULL;
  }

}
