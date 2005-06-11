//
// C++ Interface: adm_command
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ADM_script/adm_scanner.h"

typedef int (*dummy_func)(int n,Arg *args);
typedef struct admCommand
{
	char 		*command;
        char            *man;
	dummy_func	func;
	uint32_t	nbArgs;
	APM_TYPE	args[MAXPARAM];	// 10 params should be enough
};

int scriptLoad(int n,Arg *args);
int scriptExit(int n,Arg *args);
int scriptAppend(int n,Arg *args);
int scriptAudioProcess(int n,Arg *args);
int scriptVideoProcess(int n,Arg *args);

int scriptAudioDownsample(int n,Arg *args);
int scriptAudioResample(int n,Arg *args);
int scriptAudioNormalize(int n,Arg *args);


int scriptSaveAudio(int n,Arg *args);
int scriptGoto(int n,Arg *args);

int scriptSetFps(int n,Arg *args);

int scriptSleep(int n,Arg *args);
int scriptSetMarkerA(int n,Arg *args);
int scriptSetMarkerB(int n,Arg *args);
int scriptScanVbr(int n,Arg *args);
int scriptAudioDelay(int n,Arg *args);
int scriptFilm2Pal(int n,Arg *args);
int scriptPal2Film(int n,Arg *args);
int scriptAudioCodec(int n,Arg *args);
int scriptLoadCodec(int n,Arg *args);
int scriptSavejpeg(int n,Arg *args);
int scriptRaw(int n,Arg *args);
int scriptSave(int n,Arg *args);
int scriptSaveDVD(int n,Arg *args);
int scriptVideoCodec(int n,Arg *args);
int scriptVideoCodec2(int n,Arg *args);
int scriptSave(int n,Arg *args);
int scriptLoadFilter(int n,Arg *args);
int scriptAddVideoFilter(int n,Arg *args);
int scriptRemoveFrame(int n,Arg *args);
int scriptSaveOgm(int n, Arg *args);
int scriptIndexMpeg(int n, Arg *args);
int scriptScanBlackFrames(int n, Arg *args);

int scriptClearSegments(int n, Arg *srgs);
int scriptAddSegment(int n, Arg *srgs);
int scriptOutputFormat(int n, Arg *srgs);

int scriptMono2Stereo(int n, Arg *srgs);
int scriptStereo2mono(int n, Arg *srgs);
int scriptAudioReset(int n, Arg *srgs);

int scriptSetPostProc(int n, Arg *srgs);
int scriptAudioSource(int n, Arg *srgs);
int scriptGoToTime(int n, Arg *srgs);

// All comment must be in lower case!!!!!!!!!!!
// All comment must be in lower case!!!!!!!!!!!
// All comment must be in lower case!!!!!!!!!!!
// A number of param > 0x100 means variable, at least yy with value = yy+0x100
// For video, we need at least one : The video filter name
static const admCommand myCommands[]=
{
	
	{"load",       "load the file (filename)",   scriptLoad,1,APM_STRING},
	{"append",     "append the file (filename)",              scriptAppend,1,APM_STRING},
	
        {"clearsegments","delete all segments ().",         scriptClearSegments,0},
        {"addsegment",  "add a segment. addsegment(source video,startframe, nbframes)",                                                                                  scriptAddSegment,3,APM_NUM,APM_NUM,APM_NUM},

	{"videoprocess","set video process (on/off)",	scriptVideoProcess,1,APM_NUM},
	{"audioprocess","set audio process (on/off)",	scriptAudioProcess,1,APM_NUM},

        {"setpostproc","set postprocessing( type, strength, swapuv)",        scriptSetPostProc,3,APM_NUM},
	
	{"savejpg","save as a jpeg savejpg(filename)",		scriptSavejpeg,1,APM_STRING},
	{"saverawvideo","save a video ES (filename)",	scriptRaw,1,APM_STRING},
	
	{"savedvd","save as mpeg PS for DVD (filename)",scriptSaveDVD,1,APM_STRING},
	{"save","save the video (filename)",		scriptSave,1,APM_STRING},
	{"saveogm","save as ogm (filename)",		scriptSaveOgm,1,APM_STRING},
	
	{"setformat","set output format : AVI, ES,PS,TS,OGM,AVI_UNP",          scriptOutputFormat,1,APM_STRING},

        {"audiosource","audio source : VIDEO|MP3|AC3|WAV|NONE , filename",
                                                         scriptAudioSource,2,APM_STRING,APM_STRING},
        {"audioreset","reset all audio (on|off)",          scriptAudioReset,1,APM_NUM},
	{"audionormalize","normalize (ON|OFF)",	scriptAudioNormalize,1,APM_NUM},
	{"audiodownsample","48khz->44.1khz (ON|OFF)",	scriptAudioDownsample,1,APM_NUM},
	{"audioresample","resample to (frequency)",	scriptAudioResample,1,APM_NUM},
	{"audiodelay","audio delay (delay in ms)",		scriptAudioDelay,1,APM_NUM},
        {"audiomono2stereo","mono to stereo (on|off)",    scriptMono2Stereo,1,APM_NUM},
        {"audiostereo2mono","stereo to mono (on|off)",     scriptStereo2mono,1,APM_NUM},

	{"film2pal","film to pal (on|off)",		scriptFilm2Pal,1,APM_NUM},
        {"pal2film","pal to film (on|off)",            scriptPal2Film,1,APM_NUM},
	
	
	
	{"saveaudio","save audio (filename)",		scriptSaveAudio,1,APM_STRING},
	{"audiocodec","select audio codec (lame|aac|vorbis|mp3|ac3|toolame|none	,bitrate)",	   
                                scriptAudioCodec,2,APM_STRING,APM_NUM},
	{"videocodec","select video codec (Xvid|Xvid4|VCD|SVCD|DVD|X264|FFV1|FFHUFF|XDVD|XSVCD, CQ=X|CBR=X|2pass=X|follow=X)",		
                                scriptVideoCodec,2,APM_STRING,APM_STRING},
	{"videocodec","select video codec (Xvid|Xvid4|VCD|SVCD|DVD|X264|FFV1|FFHUFF|XDVD|XSVCD, CQ=X|CBR=X|2pass=X|follow=X,codec configuration file)",
                        	scriptVideoCodec2,3,APM_STRING,APM_STRING,APM_STRING},
	
	{"loadfilter","Load filter xml file (filename)",		scriptLoadFilter,1,APM_STRING},
	
	{"scanvbr","Build VBR timemap ()",		scriptScanVbr,0},
	//{"loadcodec",		scriptLoadCodec,1,APM_STRING},
	
	{"goto","Jump to frame (framenum).",		scriptGoto,1,APM_NUM},
        {"gototime","Jump to time (hh,mm,ss)",            scriptGoToTime,3,APM_NUM,APM_NUM,APM_NUM},
	
	{"setfps","change frame per second (xx.xx)",		scriptSetFps,1,APM_FLOAT},
	
	{"sleep","wait a bit (wait_seconds).",		scriptSleep,1,APM_NUM},
	
	{"setmarkera","set marker A (framenum).",		scriptSetMarkerA,1,APM_NUM},
	{"setmarkerb","set marker B (framenum).",		scriptSetMarkerB,1,APM_NUM},
	
	{"addvideofilter","Add a video filter (filtername,......)",	scriptAddVideoFilter,0x101,APM_STRING},
	{"removeframes","remove frames (framestart, frameEnd).",	scriptRemoveFrame,2,APM_NUM,APM_NUM},
        
        {"indexmpeg","index a mpeg file (file to index, index filename, audio track).",
                                                           scriptIndexMpeg,3,APM_STRING,APM_STRING,APM_NUM},
	{"scanblackframes","build a file containing all black frames (filename).",     scriptScanBlackFrames,1,APM_STRING},

	{"exit","exit avidemux2 ().", 		scriptExit,0}
};
