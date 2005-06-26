#ifndef DIA_IDX_PG
#define DIA_IDX_PG
#include "ADM_toolkit/TLK_clock.h"

class DIA_progressIndexing
{
protected:
                Clock   clock;
        
public:
                        DIA_progressIndexing(char *name);
                        ~DIA_progressIndexing();
          uint8_t       update(uint32_t done,uint32_t total, uint32_t nbImage, uint32_t hh, uint32_t mm, uint32_t ss);

};

#endif
