//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: Mean fixounet@free.Fr, (C) 2005 GPL 2
//
// Copyright: See COPYING file that comes with this distribution
//      This tries to implement bufferd output io
//      to speed up things a bit
//
#ifndef ADM_FILE_IO
#define ADM_FILE_IO



class ADMFile
{
protected:
        FILE            *_out;
        uint32_t        _fill;
        uint8_t         *_buffer;	  
        uint64_t        _curPos;
public:
                        ADMFile();
                        ~ADMFile();
        uint8_t         open(FILE *in);
        uint8_t         write(uint8_t *in, uint32_t size);
        uint8_t         flush(void);
        uint8_t         seek(uint64_t where);
        uint64_t        tell(void);
	  


};

#endif
