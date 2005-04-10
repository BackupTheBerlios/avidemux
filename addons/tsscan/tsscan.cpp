/***************************************************************************
  Small scanner for mpeg TS stream
    
            
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define uint8_t unsigned char
#define uint32_t unsigned long int

#define TS_PACKET 188
#define VERSION 0.1
int sync( FILE *fd);
int parse(FILE *fd,FILE *fo,int pid);
int usage(void );
int push(int pid, int streamid);

uint32_t stack[100];
uint32_t stacked=0;

uint8_t buffer[TS_PACKET*4];

//_____________________________
//_____________________________
int main(int argc, char **argv)
{
FILE *fd=NULL;
FILE *fo=NULL;
int pid=0;
	usage();
	if(!argv[1]) exit(0);
	
	fd=fopen(argv[1],"rb");
	if(!fd)
	{
		
			printf("Cannot open %s\n",argv[1]);
	}
	if(argc>3)
	{
		pid=atoi(argv[2]);
		if(!pid)
		{
			printf("pid must be non null\n");
			exit(0);
		}
		fo=fopen(argv[3],"wb");
		if(!fo)
		{
			printf("Cannot output to file\n");
			exit(0);
		
		}
	}
	if(!sync(fd)) { printf("Cannot find sync\n");exit(-1);}
	parse(fd,fo,pid);
	fclose(fd);
	if(fo) fclose(fo);
	printf("\nDone.\n");

}

int sync( FILE *fd)
{
int n=0;

	fread(buffer,TS_PACKET,3,fd);
	while(n<TS_PACKET*2)
	{
		if(buffer[n]==0x47 && buffer[n+TS_PACKET]==0x47)
		{
			fseek(fd,n,SEEK_SET);
			return 1;
		}
		n++;
	}
	return 0;
}
// Parse until we find a PES header to identify the stream
//
int parse(FILE *fd,FILE *fo, int cpid)
{
int loop=0;
int pid,adapt,start,len;

	while(loop<10000)
	{
		loop++;		
		if(1!=fread(buffer,TS_PACKET,1,fd))
		{
			printf("Read error\n");
			 return 0;
		}
		if(buffer[0]!=0x47)
		{
			printf("Sync lost\n");
			return 0;
		}
		pid=((buffer[1]<<8)+buffer[2]) & 0x1FFF;
		//if(pid<0x10) continue; // PAT etc...
		if(!pid)
		{
			printf("Pat found\n");
			continue;
		}
		if((pid==cpid ) && fo)
		{
			fwrite(buffer,TS_PACKET,1,fo);
		}
		adapt=(buffer[3]&0x30)>>4;
		if(!(adapt&1)) continue; // no payload
		
		start=4;
		len=184;
		if(adapt==3) //payload + adapt
		{			
			start+=1+buffer[4];
			len-=1+buffer[4];
		}
		
		// We got a packet of the correct pid
		if(!(buffer[1]&0x40)) // PES header ?
			continue;
		
		
		if(buffer[start] || buffer[start+1] || buffer[start+2]!=01)
		{
			printf("Weird...%x :%x %x %X\n",pid,buffer[start],buffer[start+1],buffer[start+2]);
			continue;		
		}
		push(pid,buffer[start+3]);
	}	
	return 1;
}
		
int usage(void)
{
	printf("\nSimple Mpeg Ts stream scanner\n");
	printf("\nVersion %1.2f by mean\n",VERSION);
	printf("\n\ntsscan filein [pid in decimal fileout]\n");
	return 1;
}
int push(int pid, int streamid)
{
uint32_t key;
	key=(pid<<16)+streamid;
	for(int i=0;i<stacked;i++)
	{
		if(stack[i]==key) return 1;
	}
	stack[stacked++]=key;
	printf("Found pid: %x with stream : %x\n",pid,streamid);
	return 1;

}
