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
#include <string.h>
#include <ADM_assert.h>
#include <math.h>
#include "default.h"
#include "ADM_fileio.h"

#define ADM_FILE_BUFFER 4*256*1024 // 256 kB

ADMFile::ADMFile( void)
{
        _out=NULL;
        _fill=0;
        _curPos=0;
        _buffer=new uint8_t[ADM_FILE_BUFFER];
        ADM_assert(_buffer);
        
}
ADMFile::~ADMFile()
{
        flush();
        if(_buffer) 
                delete [] _buffer;
        _buffer=NULL;
}
uint8_t ADMFile::open(FILE *in)
{
        ADM_assert(!_out);
        ADM_assert(in);
        _out=in;
        _curPos=ftello(_out);
        _fill=0;
        return 1;
}
uint8_t ADMFile::flush(void)
{
        if(_fill)
        {
                fwrite(_buffer,_fill,1,_out);
                _curPos+=_fill;
                _fill=0;
        }
        return 1;
}
uint64_t ADMFile::tell(void)
{
        return _curPos+_fill;  
}
uint8_t ADMFile::seek(uint64_t where)
{
        flush();
        fseeko(_out,where,SEEK_SET);
        _curPos=where;
        return 1;
}
//
// Buffered write
//
uint8_t ADMFile::write(uint8_t *data,uint32_t how)
{
        uint32_t oneshot;
        
        while(1)
        {
                ADM_assert(_fill<ADM_FILE_BUFFER);
                oneshot=_fill+how;
                if(oneshot<ADM_FILE_BUFFER)
                {
                        memcpy(_buffer+_fill,data,how);
                        _fill+=how;
                        return 1;
                }
                // copy what's possible
                oneshot=ADM_FILE_BUFFER-_fill;
                memcpy(_buffer+_fill,data,oneshot);
                _fill+=oneshot;
                flush();
                data+=oneshot;
                how-=oneshot;
        }

        return 1;
}
