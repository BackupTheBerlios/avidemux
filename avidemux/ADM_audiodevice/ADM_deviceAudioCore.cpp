/***************************************************************************
                          ADM_deviceoss.cpp  -  description
                             -------------------
    begin                : Sat Sep 28 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
  
#include "fourcc.h" 
 
#ifdef CONFIG_DARWIN
#include "ADM_library/default.h"
#include <pthread.h>

#include "ADM_toolkit/toolkit.hxx"
#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/DefaultAudioOutput.h>

#include <ADM_assert.h>
#include "ADM_audiodevice.h"
#include "ADM_audiodevice/ADM_deviceAudioCore.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME  MODULE_ADEVICE
#include "ADM_toolkit/ADM_debug.h"

static Component 		comp=NULL;
static int16_t  		*audioBuffer=NULL;
static uint32_t 		frameCount=0;
static AudioUnit 		theOutputUnit;
static uint32_t			rd_ptr=0;
static uint32_t			wr_ptr=0;
static pthread_mutex_t		lock;
#define BUFFER_SIZE (2*48000)
//_______________________________________________
//
//_______________________________________________

static OSStatus	MyRenderer(void *inRefCon, AudioUnitRenderActionFlags inActionFlags, 
	const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, 
	AudioBuffer *ioData);
static OSStatus OverloadListenerProc(	AudioDeviceID			inDevice,
		UInt32					inChannel,
		Boolean					isInput,
		AudioDevicePropertyID			inPropertyID,
		void*					inClientData);

//_______________________________________________
//
//_______________________________________________
	
OSStatus OverloadListenerProc(	AudioDeviceID			inDevice,
		UInt32					inChannel,
		Boolean					isInput,
		AudioDevicePropertyID			inPropertyID,
		void*					inClientData)
{
	printf ("* * * * * Overload detected on device playing audio\n");
	return noErr;
}
//_______________________________________________
//
//_______________________________________________

coreAudioDevice::coreAudioDevice(void) 
{
	printf("Creating Darwin coreAudio device\n");
	_inUse=0;
	pthread_mutex_init(&lock,NULL);
	pthread_mutex_unlock(&lock);
}
//_______________________________________________
//
//_______________________________________________
uint8_t  coreAudioDevice::stop(void) 
{
	if(audioBuffer)
	{
		delete [] audioBuffer;
		audioBuffer=NULL;
	}
	if(_inUse)
	{
		verify_noerr(AudioOutputUnitStop(theOutputUnit));
	}
	// Clean up
	CloseComponent(theOutputUnit);
	_inUse=0;
    	return 1;
}
//_______________________________________________
//
//_______________________________________________



OSStatus	MyRenderer(	void 				*inRefCon, 
				AudioUnitRenderActionFlags 	inActionFlags, 
				const AudioTimeStamp 		*inTimeStamp, 
				UInt32 				inBusNumber, 
				AudioBuffer  			*ioData)
{
	pthread_mutex_lock(&lock);
	uint32_t nb_sample=ioData->mDataByteSize>>1;
	uint32_t left=0;
	uint8_t *in,*out;

	in=(uint8_t *)&audioBuffer[rd_ptr];
	out=(uint8_t *)ioData->mData;
	aprintf("Audio Core: Fill : rd %lu wr:%lu nb asked:%lu \n",rd_ptr,wr_ptr,nb_sample);
	if(wr_ptr>rd_ptr)
	{
		left=wr_ptr-rd_ptr-1;	
		if(left>nb_sample)
		{
			memcpy(out,in,nb_sample*2);
			rd_ptr+=nb_sample;
		}
		else
		{
			memcpy(out,in,left*2);
			memset(out+left*2,0,(nb_sample-left)*2);
			rd_ptr+=left;
		}
	}
	else
	{
		// wrap
		left=BUFFER_SIZE-rd_ptr-1;
		if(left>nb_sample)
		{
			memcpy(out,in,nb_sample*2);
			rd_ptr+=nb_sample;
		}
		else
		{
			memcpy(out,in,left*2);
			out+=left*2;
			rd_ptr=0;
			in=(uint8_t *)&audioBuffer[0];
			nb_sample-=left;
			if(nb_sample>wr_ptr-1) nb_sample=wr_ptr-1;
			memcpy(out,in,nb_sample*2);
			rd_ptr=nb_sample;	
		}
	}
	pthread_mutex_unlock(&lock);
	return 0;
}
//_______________________________________________
//
//
//_______________________________________________
uint8_t coreAudioDevice::init(uint8_t channels, uint32_t fq) 
{
_channels = channels;
OSStatus 		err;
ComponentDescription 	desc;
AudioUnitInputCallback 	input;
AudioStreamBasicDescription streamFormat;
AudioDeviceID 		theDevice;
UInt32			sz=0;
UInt32			kFramesPerSlice=512; 

	desc.componentType = 'aunt';
	desc.componentSubType = kAudioUnitSubType_Output;
	desc.componentManufacturer = kAudioUnitID_DefaultOutput;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
		
	comp= FindNextComponent(NULL, &desc);
	if (comp == NULL)
	{
		printf("coreAudio: Cannot find component\n");
		return 0;
	}
		
	err = OpenAComponent(comp, &theOutputUnit);
	if(err)
	{
		printf("coreAudio: Cannot open component\n");
		return 0;
	}
	// Initialize it
	verify_noerr(AudioUnitInitialize(theOutputUnit));
	
	// Set up a callback function to generate output to the output unit
#if 1
	input.inputProc = MyRenderer;
	input.inputProcRefCon = NULL;
	
	verify_noerr(AudioUnitSetProperty(theOutputUnit, 
					kAudioUnitProperty_SetInputCallback, 
					kAudioUnitScope_Global,
					0,
					&input, 
					sizeof(input)));
#endif
	streamFormat.mSampleRate = fq;		
	streamFormat.mFormatID = kAudioFormatLinearPCM;	
	streamFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger 
								| kLinearPCMFormatFlagIsBigEndian
								| kLinearPCMFormatFlagIsPacked;

	streamFormat.mBytesPerPacket = channels * sizeof (UInt16);	
	streamFormat.mFramesPerPacket = 1;	
	streamFormat.mBytesPerFrame = channels * sizeof (UInt16);		
	streamFormat.mChannelsPerFrame = channels;	
	streamFormat.mBitsPerChannel = sizeof (UInt16) * 8;	
	
	verify_noerr(AudioUnitSetProperty(
		theOutputUnit,
		kAudioUnitProperty_StreamFormat,
		kAudioUnitScope_Input,
		0,
		&streamFormat,
		sizeof(AudioStreamBasicDescription)));
	
	printf("Rendering source:\n\t");
	printf ("SampleRate=%f,", streamFormat.mSampleRate);
	printf ("BytesPerPacket=%ld,", streamFormat.mBytesPerPacket);
	printf ("FramesPerPacket=%ld,", streamFormat.mFramesPerPacket);
	printf ("BytesPerFrame=%ld,", streamFormat.mBytesPerFrame);
	printf ("BitsPerChannel=%ld,", streamFormat.mBitsPerChannel);
	printf ("ChannelsPerFrame=%ld\n", streamFormat.mChannelsPerFrame);

	sz=sizeof (theDevice);
	verify_noerr(AudioUnitGetProperty 
		(theOutputUnit, kAudioOutputUnitProperty_CurrentDevice, 0, 0, &theDevice, &sz));
	sz = sizeof (kFramesPerSlice);
	verify_noerr(AudioDeviceSetProperty(theDevice, 0, 0, false,
		kAudioDevicePropertyBufferFrameSize, sz, &kFramesPerSlice));

	sz = sizeof (kFramesPerSlice);
	verify_noerr(AudioDeviceGetProperty(theDevice, 0, false, 
		kAudioDevicePropertyBufferFrameSize, &sz, &kFramesPerSlice));

	verify_noerr (AudioDeviceAddPropertyListener(theDevice, 0, false,
		kAudioDeviceProcessorOverload, OverloadListenerProc, 0));

	printf ("size of the device's buffer = %ld frames\n", kFramesPerSlice);
	
	frameCount=0;
	
	audioBuffer=new int16_t[BUFFER_SIZE]; // between hald a sec and a sec should be enough :)
	
    return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t coreAudioDevice::play(uint32_t len, float *data)
 {
 	// First put stuff into the buffer
	uint8_t *src;
	uint32_t left;

	dither16bit(len, data);

	pthread_mutex_lock(&lock);

	// Check we have room left
	if(wr_ptr>=rd_ptr)
	{
		left=BUFFER_SIZE-(wr_ptr-rd_ptr);
	}
	else
	{
		left=rd_ptr-wr_ptr;
	}
	if(len+1>left)
	{
		printf("AudioCore:Buffer full!\n");
		pthread_mutex_unlock(&lock);
		return 0;
	}

	// We have room left, copy it
	src=(uint8_t *)&audioBuffer[wr_ptr];
	if(wr_ptr+len<BUFFER_SIZE)
	{
		memcpy(src,data,len*2);
		wr_ptr+=len;
	}
	else
	{
		left=BUFFER_SIZE-wr_ptr-1;
		memcpy(src,data,left*2);
		memcpy(audioBuffer,data+left*2,(len-left)*2);
		wr_ptr=len-left;	
	}
	//aprintf("AudioCore: Putting %lu bytes rd:%lu wr:%lu \n",len*2,rd_ptr,wr_ptr);
	pthread_mutex_unlock(&lock);	
 	if(!frameCount)
	{
		_inUse=1;
		verify_noerr(AudioOutputUnitStart(theOutputUnit));
	}
	return 1;
}
#else
void dummy_ac_func( void);
void dummy_ac_func( void)
 {
}

#endif

