/***************************************************************************
                          automation.cpp  -  description
                             -------------------
    begin                : Thu Oct 10 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    This file reads the command line and do the corresponding command
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdlib.h>
#include <glib.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include "config.h"

#include <gtk/gtk.h>

#include "config.h"

#include "interface.h"
#include "support.h"
#include "avi_vars.h"
//#include "ADM_gui/GUI_vars.h"
#include "gui_action.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "prefs.h"
#include "gtkgui.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME  MODULE_COMMANDLINE
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_filter/vidVCD.h"
extern uint8_t loadVideoCodecConf( char *name);
extern void A_saveJpg (char *name);
extern void filterLoadXml(char *n);
extern void A_openAvi(char *name);
extern void A_appendAvi (char *name);
extern void A_saveAudio(char *name);
extern void A_saveAudioDecodedTest(char *name);
extern uint8_t indexMpeg(char *mpeg,char *file,uint8_t aid);
extern void A_SaveAudioNVideo(char *name);
extern void GUI_loadMP3(char *name);
extern void A_loadAC3(char *name);
extern void A_loadWave(char *name);
extern void GUI_Quiet( void);
extern void GUI_Verbose( void);
extern void audioFilter_SetBitrate( int i);
extern void A_Save(char *name);
extern void videoCodecSelectByName(const char *name);
extern void videoCodecConfigure(char *p,uint32_t i, uint8_t  *c);
#include "oplug_mpeg/op_mpeg.h"
extern void encoderSetLogFile(char *name)     ;
extern void A_loadWorkbench(char *name)     ;
extern void updateLoaded( void );
extern void setPostProc(int v,int s);
extern void HandleAction(Action action) ;
static  void load_workbench(char *name);;
static void call_buildtimemap( char *p);
static void call_audioproc(char *p) ;
static void call_videoproc(char *p) ;
static void call_quit(char *p) ;
static void setBegin(char *p)   ;
static void setEnd(char *p)      ;
//static void saveRawAudio(char *p)      ;
static void call_normalize(char *p) ;
static void call_resample(char *p) 	;
static void call_downsample(char *p) 	;
static void call_help(char *p) 	;
static void call_setAudio(char *p) 	;
//static void call_load(char *p) 	;
static void call_indexMpeg(char *in,char *out,char *aid) 	;
static void call_autosplit(char *p) 	;
static void call_audiobitrate(char *p) 	;
static void call_fps(char *p) 	;
static void call_audiocodec(char *p) 	;
static void call_videocodec(char *p) ;
static void call_videoconf(char *p) ;
static int searchReactionTable(char *string);
static void call_setPP(char *v,char *s);
static void call_toolame(char *p);
extern void A_loadWorkbench( char *name);
extern void updateLoaded(void );
extern void A_requantize2( float percent, uint32_t quality, char *out_name );
static void save(char*name);
static void call_requant(char *p, char *q, char *n);

//_________________________________________________________________________

extern uint8_t audioShift;
extern int32_t audioDelay;
extern uint32_t muxSize;
extern int global_argc;
extern char **global_argv;
extern void	ADM_saveRaw(char *name );
//_________________________________________________________________________

typedef 	void 			(*one_arg_type)(char *arg);
typedef	void 			(*two_arg_type)(char *arg,char *otherarg);
typedef	void 			(*three_arg_type)(char *arg,char *otherarg,char *yetother);
//_________________________________________________________________________

typedef struct AUTOMATON
{
		const char 			*string;
		uint8_t 	have_arg;
		const char			*help_string;
		one_arg_type callback;
};
//_________________________________________________________________________

AUTOMATON reaction_table[]=
{
		{"audio-process",	0,"activate audio processing",		call_audioproc},
		{"audio-normalize",	0,"activate normalization",		call_normalize},
		{"audio-downsample",	0,"activate 48->44 downsampling",	call_downsample},
		{"audio-resample",	1,"resample to x hz",			call_resample},
		
		{"video-process",	0,"activate video processing",		call_videoproc},	
		{"filters",		1,"load a filter preset",		filterLoadXml}   ,
		{"codec-conf",		1,"load a codec configuration",		(one_arg_type )loadVideoCodecConf}   ,
		{"vcd-res",		0,"set VCD resolution",			(one_arg_type)setVCD}              ,
		{"svcd-res",		0,"set SVCD resolution",		(one_arg_type)setSVCD}              ,
		{"dvd-res",		0,"set DVD resolution",			(one_arg_type)setDVD}  ,
		{"halfd1-res",		0,"set 1/2 DVD resolution",		(one_arg_type)setHalfD1} ,  
		{"save-jpg",		1,"save a jpeg",			(one_arg_type)A_saveJpg}        ,

		{"begin",		1,"set start frame",			setBegin},
		{"end",			1,"set end frame",			setEnd},
		
		{"save-raw-audio",	1,"save audio as-is ",			A_saveAudio},
		{"save-raw-video",	1,"save raw video stream (mpeg/... ) ",	ADM_saveRaw},
		{"save-uncompressed-audio",1,"save uncompressed audio",A_saveAudioDecodedTest},
		{"index-mpeg",		3,"create index of vob/mpeg : vob.vob index.index audio#",
								((one_arg_type )call_indexMpeg)},
		{"load",		1	,"load video",		A_openAvi},
		{"append",		1	,"append video",	A_appendAvi},
		{"load-workbench",	1	,"load workbench file",	load_workbench},
		{"save",		1	,"save avi",		save},		
		{"external-mp3",	1	,"load external mpeg audio as audio track",GUI_loadMP3},
		{"external-ac3",	1	,"load external ac3 audio as audio track",A_loadAC3},
		{"external-wav",	1	,"load external wav audio as audio track",A_loadWave},
		{"audio-delay",		1	,"set audio time shift in ms (+ or -)",	call_setAudio},
		{"audio-map",		0	,"build audio map (MP3 VBR)",	call_buildtimemap},
		{"audio-bitrate",	1	,"set audio encoding bitrate",	call_audiobitrate},
		{"fps",	1	,"set frames per second",	call_fps},
		{"audio-codec",		1	,"set audio codec (MP2/MP3/AC3/NONE/TOOLAME)",call_audiocodec},
		{"audio-toolame",	1	,"pipe audio to toolame and save to file",call_toolame},
		{"video-codec",		1	,"set video codec (Divx/Xvid/FFmpeg4/VCD/SVCD/DVD/XVCD/XSVCD)",				call_videocodec},
		{"video-conf",		1	,"set video codec conf (cq=q|cbr=br|2pass=size)[,mbr=br][,matrix=(0|1|2|3)]",				call_videoconf},
		{"2pass-log",		1	,"select the log file for 2 passes mode",				encoderSetLogFile},
		{"set-pp",		2	,"set post processing default value, value(1=hdeblok|2=vdeblock|4=dering) and strength (0-5)",	(one_arg_type )	call_setPP},

		{"autosplit",		1	,"split every N MBytes",call_autosplit},
		{"requant",		3	,"requantize mpeg2 : percent quality[0..3] output_name",
										(one_arg_type )call_requant},
		
		{"help",		0,"print this",		call_help},
		{"quit",		0,"exit avidemux",	call_quit}


}  ;
#define NB_AUTO (sizeof(reaction_table)/sizeof(AUTOMATON))
//_________________________________________________________________________

typedef enum {
	SOME_UNKNOWN,
	MPEG2ENC_VCD,    // pseudo codec with profile
	MPEG2ENC_SVCD,   // pseudo codec with profile
	MPEG2ENC_DVD     // pseudo codec with profile
} codec_t;
static codec_t codec = SOME_UNKNOWN;

//void automation(int argc, char **argv)
int automation(void )

{
static char **argv;
static int argc;
static int cur;
static int myargc;
static three_arg_type three;
static two_arg_type two;
static int index;
			argv=global_argv;
			argc=global_argc;
			usleep(100000); // let gtk start
			GUI_Quiet();
			printf("\n *** Automated : %d entries*************\n",NB_AUTO);
			// we need to process
			argc-=1;
			cur=1;
			myargc=argc;
			while(myargc>0)
			  		{
							 if(( *argv[cur]!='-') || (*(argv[cur]+1)!='-'))
								 {
							     	if(cur==1) 
								  	{
										 	A_openAvi(argv[cur]);
										}
									 	else
									  	printf("\n Found garbage %s\n",argv[cur]);
								  cur+=1;myargc-=1;				
								  continue;
								}
								// else it begins with --
								
								
								index= searchReactionTable(argv[cur]+2);
								if(index==-1) // not found
								{
									 			printf("\n Unknown command :%s\n",argv[cur] );
												cur+=1;myargc-=1;	
								}
								else
								{
											printf("%s-->%d\n", reaction_table[index].string,reaction_table[index].have_arg);
											
											switch(  reaction_table[index].have_arg)
											{
												case 3:
													        
													        three=(  three_arg_type) reaction_table[index].callback;
													        three( argv[cur+1],argv[cur+2],argv[cur+3]);
																	printf("\n arg: %d index %d\n",myargc,index);																        
													        break;												
												case 2:													       
													        two=(  two_arg_type) reaction_table[index].callback;
													        two( argv[cur+1],argv[cur+2]);
													        break;
												case 1:		
																	reaction_table[index].callback(argv[cur+1]);
																	break;
												case 0:
																  reaction_table[index].callback(NULL);
																  break;
												default:
																	assert(0);
																	break;																  
												} 
												cur+=1+reaction_table[index].have_arg;
												myargc-=1+reaction_table[index].have_arg;		   
								}				
												    															
					} // end while
          GUI_Verbose();
          printf("\n ********** Automation ended***********\n");
          return 0;
}
//_________________________________________________________________________

int searchReactionTable(char *string)
{
	   for(unsigned int j=0;j<NB_AUTO;j++)
	   {
			 	if(!strcmp(string,reaction_table[j].string))		return j;			 
			}
			return -1;
	
}

//_________________________________________________________________________
//_________________________________________________________________________
//_________________________________________________________________________
//_________________________________________________________________________
//_________________________________________________________________________

void call_quit        (char *p) { UNUSED_ARG(p); exit(0);                            }
void call_audioproc   (char *p) { UNUSED_ARG(p); HandleAction(ACT_AudioModeProcess); }
void call_videoproc   (char *p) { UNUSED_ARG(p); HandleAction(ACT_VideoModeProcess); }


extern  int  audioNormalizeMode;
extern void audioForceDownSample( void );
extern void audioSetResample(uint32_t fq);
void call_normalize   (char *p)
{
	audioNormalizeMode=1; // Ugly
	aprintf("normalize\n");
	 UNUSED_ARG(p);
}
void call_downsample    (char *p)
{
	audioForceDownSample();
	printf("downsample\n");
	 UNUSED_ARG(p); 

}
void call_resample    (char *p)
{
int fq;
	fq=atoi(p);
	if(fq>1000)
	{
		audioSetResample(fq);
		printf("resample to %d\n",fq);
	}
	else
	{
		printf("*** INVALID FREQUENCY***\n");
	}	 

}



void call_buildtimemap(char *p) { UNUSED_ARG(p); aprintf("timemap\n");HandleAction(ACT_AudioMap);         }

void call_setPP(char *v,char *s)
{
int iv=3,is=5;
	sscanf(v,"%d",&iv);
	sscanf(s,"%d",&is);
	aprintf("post proc %d %d\n",iv,is);
	setPostProc(iv,is);
	

}
void call_setAudio (char *p) 	
{
	
		int32_t i;
		sscanf(p,"%ld",&i);
		audioDelay=i;  
		printf("\n CLI audio delay %ld\n",audioDelay);	
		audioShift=1;	
}
void call_audiocodec(char *p)
{
	if(!strcmp(p,"MP2"))
		audioCodecSetcodec( AUDIOENC_MP2 );
	else if(!strcmp(p,"AC3"))
		audioCodecSetcodec( AUDIOENC_AC3 );
#ifdef HAVE_LIBMP3LAME		
	else if(!strcmp(p,"MP3"))
		 audioCodecSetcodec( AUDIOENC_MP3 );
#endif
	else if(!strcmp(p,"NONE"))
		audioCodecSetcodec( AUDIOENC_NONE );
	else if(!strcmp(p,"TOOLAME"))
		audioCodecSetcodec( AUDIOENC_2LAME );		
	else{
		audioCodecSetcodec( AUDIOENC_NONE );
		fprintf(stderr,"audio codec \"%s\" unknown.\n",p);
	}
}
void call_videocodec(char *p)
{
#warning fixme : Name does not match !

	videoCodecSelectByName(p);

}
static void call_videoconf(char *p)
{
	videoCodecConfigure(p,0,NULL);

}
void call_audiobitrate(char *p)
{

		int32_t i;
		sscanf(p,"%ld",&i);
		printf("\n Audio bitrate %ld\n",i);
		audioFilter_SetBitrate(i);
}
void call_fps(char *p)
{

		float fps;
		aviInfo info;

		if (avifileinfo)
		{
			video_body->getVideoInfo(&info);
			sscanf(p,"%f",&fps);
			printf("\n Frames per Second %f\n",fps);
			info.fps1000 = (uint32_t) (floor (fps * 1000.+0.49));
			video_body->updateVideoInfo (&info);
			video_body->getVideoInfo (avifileinfo);
		} else 
		{
			printf("\n No Video loaded; ignoring --fps\n");
		}
}
void call_autosplit(char *p)
{

		int32_t i;
		sscanf(p,"%ld",&i);
		muxSize=i;
		printf("\n autosplit every %lu megabytes\n",muxSize);

			}

void setBegin(char *p) 
{	
uint32_t i;
		sscanf(p,"%lu",&i);
		frameStart=i;  
		printf("\n Start %lu\n",frameStart);
}
void setEnd(char *p) 
{	
uint32_t i;
		sscanf(p,"%lu",&i);
		frameEnd=i;  
		printf("\n End %lu\n",frameStart);

}
void call_help(char *p)
{
    UNUSED_ARG(p);
	printf("\n Command line possible arguments :");
		for(unsigned int i=0;i<NB_AUTO;i++)
			{
					printf("\n    --%s, %s ", reaction_table[i].string,reaction_table[i].help_string);
					switch(reaction_table[i].have_arg)
					{
						case 0:	 printf(" ( no arg )");break;
						case 1:	 printf(" (one arg )");break;
						case 2:	 printf(" (two args )");break;
						case 3:	 printf(" (three args) ");break;

					}

			}
	
			call_quit(NULL);
}
void call_indexMpeg(char *in,char *out, char *id)
{
uint32_t aid;
					      if(id==NULL) return;
								sscanf(id,"%x",&aid);
              	indexMpeg(in,out,(uint8_t)aid);
               	printf("\n __\n");
}
void call_requant(char *p, char *q, char *n)
{

	float f;
	uint32_t i;
	f=atof(p);
	i=atoi(q);
	A_requantize2( f,i, n )	;
}
void load_workbench(char*name){
   assert(name);
   A_loadWorkbench(name);
 //  updateLoaded();
}

void save(char*name)
{
	A_Save(name);
}

void call_toolame(char *file)
{
	A_Pipe(P_TOOLAME,file);
}


//EOF
