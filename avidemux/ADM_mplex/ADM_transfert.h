//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef _ADM_TRANSFERT_
#define _ADM_TRANSFERT_
#include <pthread.h>
#define TRANSFERT_BUFFER 1024*1024*10
class admMutex
{
private:
                uint8_t _locked;
public: 
                pthread_mutex_t _tex;       
                        admMutex( void);
                        ~admMutex();        
                uint8_t lock(void);
                uint8_t unlock(void);
                uint8_t isLocked(void);
};

class admCond
{
private:
        pthread_cond_t  _cond;
        admMutex        *_condtex;

public:        
        uint8_t         waiting;
        uint8_t         aborted;
                        admCond( admMutex *tex);
                        ~admCond();        
                uint8_t wait(void);
                uint8_t wakeup(void);
                uint8_t iswaiting(void);
                uint8_t abort(void);
                
};


class Transfert
{
protected:
        admMutex mutex;
        admCond  *cond;
        admCond  *clientCond;             
        
        uint8_t  aborted;
        uint8_t  *buffer;   
        uint32_t head,tail;     
        uint8_t  dumpStatus(void);
        uint32_t transfered_r,transfered_w;
        uint32_t _minRequired;
public:        
                Transfert( uint32_t minimumBuffer );
                ~Transfert(  );
        uint32_t read(uint8_t *buf, uint32_t nb  );
        uint8_t write(uint8_t *buf, uint32_t nb  );
        uint8_t needData( void );        
        uint8_t abort( void ); 
        uint8_t eof( void ) {return aborted;}; 
        uint8_t fillingUp( void);
        uint8_t clientLock( void);
        uint8_t audioEof(void );
        
};

#endif
//EOF

