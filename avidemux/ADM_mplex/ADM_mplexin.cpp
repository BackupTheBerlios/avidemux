//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audio/ADM_a52info.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_editor/ADM_Video.h"

#include "ADM_lavformat/ADM_lavformat.h"


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

FileOutputStream::FileOutputStream( const char *name_pat ) 
{
        strncpy( filename_pat, name_pat, MAXPATHLEN );
        snprintf( cur_filename, MAXPATHLEN, filename_pat, segment_num );
}
      
int FileOutputStream::Open()
{
        strm = fopen( cur_filename, "wb" );
        if( strm == NULL )
        {
                mjpeg_error_exit1( "Could not open for writing: %s", cur_filename );
        }

        return 0;
}

void FileOutputStream::Close()
{ 
    fclose(strm);
}


off_t
FileOutputStream::SegmentSize()
{
        struct stat stb;
    fstat(fileno(strm), &stb);
        off_t written = stb.st_size;
    return written;
}

void 
FileOutputStream::NextSegment( )
{

    
        fclose(strm);
        ++segment_num;
    
        cur_filename[strlen(cur_filename)-1]++; // increase
        strm = fopen( cur_filename, "wb" );
        if( strm == NULL )
        {
                mjpeg_error_exit1( "Could not open for writing: %s", cur_filename );
        }

        
}

void
FileOutputStream::Write( uint8_t *buf, unsigned int len )
{
    if( fwrite( buf, 1, len, strm ) != len )
    {
        mjpeg_error_exit1( "Failed write: %s", cur_filename );
    }
}

