#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>


#undef malloc
#undef realloc
#undef free
#include <ADM_assert.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"



#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_LAVFORMAT
#include "ADM_toolkit/ADM_debug.h"

#include "cpu_accel.h"
#include "mjpeg_types.h"
#include "mjpeg_logging.h"
#include "mpegconsts.h"

#include "interact.hpp"
#include "bits.hpp"
#include "outputstrm.hpp"
#include "multiplexor.hpp"

#include "ADM_inout.h"
#include "ADM_transfert.h"


/******************************************************************************/
/********************************
 *
 * IFileBitStream - Input bit stream class for bit streams sourced
 * from standard file I/O (this of course *includes* network sockets,
 * fifo's, et al).
 *
 * OLAF: To hook into your PES reader/reconstructor you need to define
 * a class like this one, where 'ReadStreamBytes' calls you code to
 * generate the required number of bytes of ES data and transfer it 
 * to the specified buffer.  The logical way to do this would be to
 * inherit IBitStream as a base class of the top-level classes for the ES
 * reconstructors.
 *
 ********************************/

bool IFileBitStream::EndOfStream(void) 
{
        if(_transfert->eof()) return true;
        return false;

}

IFileBitStream::IFileBitStream(Transfert *trans,
                                unsigned int buf_size) :
    IBitStream()
{
        _transfert=trans;        
        SetBufSize(buf_size);
        eobs = false;
        byteidx = 0;
        
        if (!ReadIntoBuffer())
        {
                        ADM_assert(buffered);
                
        }
        
}


/**
   Destructor: close the device containing the bit stream after a read
   process
*/
IFileBitStream::~IFileBitStream()
{
        
        
        Release();
}
/**

*/
 size_t IFileBitStream::ReadStreamBytes( uint8_t *buf, size_t number )
 {
        return _transfert->read(buf,number);
 }
 //EOF
 