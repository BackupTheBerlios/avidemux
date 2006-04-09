#include "config.h"
#ifdef CYG_MANGLING
#include "windows.h"
#include "winbase.h"
#include "io.h"
#include "winsock2.h"
#define WIN32_CLASH
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include <errno.h>
#include <ADM_assert.h>

#include "ADM_avsproxy_internal.h"
#include "ADM_avsproxy.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_3GP
#include "ADM_toolkit/ADM_debug.h"

#define MAGGIC 0xDEADBEEF

uint8_t avsHeader::bindMe(uint32_t port)
{
 #ifndef CYG_MANGLING
 mySocket = socket(PF_INET, SOCK_STREAM, 0);
 #else
 mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 #endif
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
    service.sin_port = htons(port);
    
    if(connect(mySocket,(struct sockaddr *)&service,sizeof(service)))
    {
        printf("Socket connect %d\n",errno);
        return 0;
    }
    return 1;
}
uint8_t avsHeader::close(void)
{
    if(mySocket)
    {
        int er;
#ifndef CYG_MANGLING        
        er=shutdown(mySocket,SHUT_RDWR);
#else
        er=shutdown(mySocket,SD_BOTH);
#endif
        if(er) printf("Error when socket shutdown  %d\n",er);
        mySocket=0;
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
