/*

(c) Mean 2006
*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "math.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include <ADM_assert.h>
#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"


#include "ADM_library/fourcc.h"
#include "ADM_avsproxy/ADM_avsproxy.h"
#include "ADM_avsproxy/ADM_avsproxy_internal.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_3GP
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_toolkit/TLK_clock.h"
#define MAGGIC 0xDEADBEEF

avsHeader::avsHeader()
{
    mySocket=0;
}
 avsHeader::~avsHeader(  )
{
    close();   
}
uint8_t avsHeader::close(void)
{
    if(mySocket)
    {
        int er;
        er=shutdown(mySocket,SHUT_RDWR);
        if(er) printf("Error when socket shutdown  %d\n",er);
        mySocket=0;
    }
    return 1;
}

uint8_t avsHeader::open(char *name)
{
    mySocket = socket(PF_INET, SOCK_STREAM, 0);
    if(mySocket==-1)
    {
        printf("Socket failed\n");
        return 0;
    }
    struct sockaddr_in  service;
    service.sin_family = AF_INET;
#ifdef DEBUG_NET
    service.sin_addr.s_addr = inet_addr("192.168.0.10");
#else
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif    
    service.sin_port = htons(9999);
    
    if(connect(mySocket,(struct sockaddr *)&service,sizeof(service)))
    {
        printf("Socket connect %d\n",errno);
        return 0;
    }
    // now time to grab some info
    avsInfo info;
    if(!askFor(AvsCmd_GetInfo,0,sizeof(info),(uint8_t*)&info))
    {
        printf("Get info failed\n");
        return 0;   
    }
    // Build header..
    _isaudiopresent = 0;	// Remove audio ATM
    _isvideopresent = 1;	// Remove audio ATM

#define CLR(x)              memset(& x,0,sizeof(  x));

    CLR(_videostream);
    CLR(_mainaviheader);

    _videostream.dwScale = 1000;
    _videostream.dwRate = info.fps1000;
    _mainaviheader.dwMicroSecPerFrame = 40000;;	// 25 fps hard coded
    _videostream.fccType = fourCC::get((uint8_t *) "YV12");

    _video_bih.biBitCount = 24;

    _videostream.dwLength = _mainaviheader.dwTotalFrames = info.nbFrames;
    _videostream.dwInitialFrames = 0;
    _videostream.dwStart = 0;
    //
    //_video_bih.biCompression= 24;
    //
    _video_bih.biWidth = _mainaviheader.dwWidth = info.width;
    _video_bih.biHeight = _mainaviheader.dwHeight = info.height;
    _video_bih.biCompression = _videostream.fccHandler =  fourCC::get((uint8_t *) "YV12");
    
    printf("Connection to avsproxy succeed\n");
    return 1;
}



uint8_t  avsHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
    uint32_t page=(_mainaviheader.dwWidth*_mainaviheader.dwHeight*3)>>1;
    *framelen=page;
    
    if(framenum>=_mainaviheader.dwTotalFrames)
    {
        printf("Avisynth proxy out of bound %u / %u\n",framenum,_mainaviheader.dwTotalFrames);
        return 0;
    }
    if(!askFor(AvsCmd_GetFrame,framenum,page,ptr))
    {
        printf("Get frame failed for frame %u\n",framenum);
        return 0;   
    }
    
    return 1;
}
uint8_t avsHeader::askFor(uint32_t cmd,uint32_t frame, uint32_t payloadsize,uint8_t *payload)
{
   
    if(!sendData(cmd,frame,0,NULL))
    {
        printf("Send Cmd %u failed for frame %u\n",cmd,frame);
        return 0;
    }
    // Wait reply
    uint32_t size,reply,outframe;
    if(!receiveData(&reply,&outframe,&size,payload))
    {
        printf("Rx Cmd %u failed for frame %u\n",cmd,frame);
        return 0;   
    }
  
    // Check!
    ADM_assert(outframe==frame);
    ADM_assert(reply==cmd+1);
    ADM_assert(size==payloadsize);
    aprintf("Cmd %u on frame %u succeed\n",cmd,frame);
    return 1;
    
}
uint8_t avsHeader::receiveData(uint32_t *cmd, uint32_t *frame,uint32_t *payload_size,uint8_t *payload)
{
        SktHeader header;
        memset(&header,0,sizeof(header));


        if( sizeof(header)!=recv(mySocket,(char *)&header,sizeof(header),0))
        {
            printf("Error in receivedata: header\n");
            exit(-1);
        }
        *cmd=header.cmd;
        *payload_size=header.payloadLen;
        *frame=header.frame;
        if(header.magic!=(uint32_t)MAGGIC)
        {
            printf("Wrong magic\n");
            exit(-1);
        }
        if(header.payloadLen)
        {
            int togo=header.payloadLen;
            int chunk;
            while(togo)
            {
                chunk=recv(mySocket,(char *)payload,togo,0);
                if(chunk<0)
                {
                    printf("Error in receivedata: body\n");
                    exit(-1);
                }
                togo-=chunk;
                payload+=chunk;
            }
        }

        return 1;
}


uint8_t avsHeader::sendData(uint32_t cmd,uint32_t frame, uint32_t payload_size,uint8_t *payload)
{
        SktHeader header;
        memset(&header,0,sizeof(header));

        header.cmd=cmd;
        header.payloadLen=payload_size;
        header.frame=frame;
        header.magic=(uint32_t)MAGGIC;
	
        if(sizeof(header)!= send(mySocket,(char *)&header,sizeof(header),0))
        {
            printf("Error in senddata: header\n");
            exit(-1);
        }
        int togo=payload_size;
        int chunk;
        while(togo)
        {
            chunk=send(mySocket,(char *)payload,togo,0);
            if(chunk<0)
            {
                printf("Error in senddata: body\n");
                exit(-1);
            }
            togo-=chunk;
            payload+=chunk;
        }
        return 1;
}



