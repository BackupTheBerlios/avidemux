// Avs2YUV by Loren Merritt

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "internal.h"
#include "sket.h"
#include "avsHeader.h"

#ifdef _MSC_VER
// what's up with MS's std libs?
#define dup _dup
#define popen _popen
#define pclose _pclose
#define fdopen _fdopen
#define setmode _setmode
#else
#include <unistd.h>
#endif

#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif

#define MY_VERSION "Avs2YUV 0.24 ADM_0.1"
#define MAX_FH 10

#include "winsock2.h"
	int vid_width=-1;
	int vid_height=-1;
	int vid_fps1000=-1;
	int vid_nbFrame=-1;
	int currentFrame=0;
	char *Buffer;

static int framePack(PVideoFrame p);
PVideoFrame Aframe=NULL;

int __cdecl main(int argc, const char* argv[])
{
	const char* infile = NULL;
	avsyInfo    info;
	HMODULE avsdll;

	printf("AvsSocket Proxy, derivated from avs2yuv by  Loren Merritt \n");
	
	
		if(argc>=2)
		{
			infile = argv[1];
			const char *dot = strrchr(infile, '.');
			if(!dot || strcmp(".avs", dot))
			{
				fprintf(stderr, "infile (%s) doesn't look like an avisynth script\n", infile);
				infile=NULL;
			}
		}
		else
		{
			infile="toto.avs";
		}
	if(!infile) {
		fprintf(stderr, MY_VERSION "\n"
		"Usage: avs2yuv  in.avs \n");
		return 2;
	}
	//********************************
	WSADATA wsaData;
	int iResult;
		printf("Initializing WinSock\n");
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != NO_ERROR)
		{
			printf("Error at WSAStartup()\n");
			exit(-1);
		}	
		printf("WinSock ok\n");
	//********************************
		printf("Loading Avisynth.dll \n");
		avsdll = LoadLibrary("avisynth.dll");
		if(!avsdll) {fprintf(stderr, "failed to load avisynth.dll\n"); return 2;}
		printf("Avisynth.dll loaded\n");
		IScriptEnvironment* (* CreateScriptEnvironment)(int version)
			= (IScriptEnvironment*(*)(int)) GetProcAddress(avsdll, "CreateScriptEnvironment");
		printf("Env created\n");
		if(!CreateScriptEnvironment)
			{fprintf(stderr, "failed to load CreateScriptEnvironment()\n"); return 1;}
	
		IScriptEnvironment* env = CreateScriptEnvironment(AVISYNTH_INTERFACE_VERSION);
		AVSValue arg(infile);
		printf("Importing..\n");
		AVSValue res = env->Invoke("Import", AVSValue(&arg, 1));
		printf("Importing ok\n");
		if(!res.IsClip())
			{fprintf(stderr, "Error: '%s' didn't return a video clip.\n", infile); return 1;}
		PClip clip = res.AsClip();
		VideoInfo inf = clip->GetVideoInfo();
	
		info.width=vid_width=inf.width;
		info.height=vid_height=inf.height;
		info.fps1000=vid_fps1000=(inf.fps_numerator*1000)/inf.fps_denominator;
		info.nbFrames=vid_nbFrame=inf.num_frames;
		
		if(!inf.IsYV12()) 
		{
			fprintf(stderr, "converting %s -> YV12\n", inf.IsYUY2() ? "YUY2" : inf.IsRGB() ? "RGB" : "?");
			res = env->Invoke("converttoyv12", AVSValue(&res, 1));
			clip = res.AsClip();
			inf = clip->GetVideoInfo();
		}
		if(!inf.IsYV12())
			{fprintf(stderr, "Couldn't convert input to YV12\n"); return 1;}
		if(inf.IsFieldBased())
			{fprintf(stderr, "Needs progressive input\n"); return 1;}

	
		// Incoming ready
		printf("Info\n");
		printf("Width   :%d \n",vid_width);
		printf("Height  :%d \n",vid_height);
		printf("Fps1K   :%d \n",vid_fps1000);
		printf("NbFrame :%d \n",vid_nbFrame);
				
		Buffer=new char[vid_width*vid_height*2]; // Too much but who cares
		
		//
		Sket *sket=new Sket();
		if(!sket->waitConnexion())
		{
			printf("Accept/listen error\n");
			exit(-1);
		}
		// We got a connection, now handle request
		// First be stupid
		uint32_t cmd,frame,len;
		uint8_t payload[1000]; // Never used normally...
		while(1)
		{
			if(!sket->receive(&cmd,&frame,&len,payload))
			{
				printf("Error in receive\n");
				exit(-1);
			}
			switch(cmd)
			{
					case AvsCmd_GetInfo:
							printf("Received get info...\n");
							sket->sendData(AvsCmd_SendInfo,0,sizeof(info),(uint8_t *)&info);
							break;
					case AvsCmd_GetFrame:
							

							printf("Get frame %u\n",frame);
							Aframe= clip->GetFrame(frame, env);
							framePack(Aframe);
							sket->sendData(AvsCmd_SendFrame,0,sizeof(info),(uint8_t *)Buffer);
							break;
					default:
							printf("Unknown command\n");
							exit(-1);


			}


		}

		//get(currentFrame);
	
}

int framePack(PVideoFrame p)
{
			int w,stride;
			const BYTE *data;
			char *out;
			
			
			data=p->GetReadPtr(PLANAR_Y);
			w=vid_width;
			stride=p->GetPitch(PLANAR_Y);
			out=Buffer;
			for(int i=0;i<vid_height;i++)
			{
				memcpy(out,data,w);
				data+=stride;
				out+=w;
			}

			data=p->GetReadPtr(PLANAR_U);
			w=vid_width>>1;
			stride=p->GetPitch(PLANAR_U);
			out=Buffer+(vid_width*vid_height);
			for(int i=0;i<vid_height;i++)
			{
				memcpy(out,data,w);
				data+=stride>>1;
				out+=w;
			}

			data=p->GetReadPtr(PLANAR_V);
			w=vid_width>>1;
			stride=p->GetPitch(PLANAR_V);
			out=Buffer+((3*vid_width*vid_height)>>1);
			for(int i=0;i<vid_height;i++)
			{
				memcpy(out,data,w);
				data+=stride>>1;
				out+=w;
			}
			return 1;
}

		
	

//EOF

