//
// C++ Implementation: ADM_deviceWin32
//
// Description: 
// C++ Implementation: ADM_deviceWin32
// Use MM layer to output sound
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//

#include "config.h"

#ifdef __WIN32
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <windows.h>

#include "fourcc.h" 
#include "ADM_assert.h"
#include "ADM_audiodevice.h"
#include "ADM_audiodevice/ADM_deviceWin32.h"

#define MODULE_NAME  MODULE_ADEVICE
#include "ADM_osSupport/ADM_debugID.h"
#include "ADM_osSupport/ADM_debug.h"

#define NB_BUCKET 8

static uint32_t bucketSize;
static HWAVEOUT myDevice;
static MMRESULT myError;

static void handleMM(MMRESULT err);

WAVEHDR waveHdr[NB_BUCKET];	

win32AudioDevice::win32AudioDevice(void) 
{
	printf("[Win32] Creating audio device\n");
	_inUse=0;
}

uint8_t win32AudioDevice::stop(void) 
{
	if (!_inUse)
		return 0;

	printf("[Win32] Closing audio\n");

	waveOutReset(myDevice);		

	for (uint32_t i = 0; i < NB_BUCKET; i++)
		waveOutUnprepareHeader(myDevice, &waveHdr[i], sizeof(WAVEHDR));

	myError = waveOutClose(myDevice);

	if (myError != MMSYSERR_NOERROR)
	{
		printf("[Win32] Close failed %d\n", myError);
		handleMM(myError);
		return 0;
	}

	for (uint32_t i = 0; i < NB_BUCKET; i++)
		delete[] waveHdr[i].lpData;

	_inUse=0;
	myDevice = NULL;

	return 1;
}

uint8_t win32AudioDevice::init(uint8_t channels, uint32_t fq) 
{
	printf("[Win32] Opening Audio, channels=%u freq=%u\n",channels, fq);

	if (_inUse) 
	{
		printf("[Win32] Already running?\n");
		return 0;
	}

	_inUse = 1;
	_channels = channels;
	bucketSize = channels * fq;

	WAVEFORMATEX wav;

	memset(&wav, 0, sizeof(WAVEFORMATEX));

	wav.wFormatTag = WAVE_FORMAT_PCM;
	wav.nSamplesPerSec = fq;
	wav.nChannels = channels;
	wav.nBlockAlign = 2 * channels;
	wav.nAvgBytesPerSec = 2 * channels * fq;
	wav.wBitsPerSample = 16;

	myError = waveOutOpen(&myDevice, WAVE_MAPPER, &wav, NULL, NULL, CALLBACK_NULL);

	if (MMSYSERR_NOERROR != myError)
	{
		printf("[Win32] waveOutOpen failed\n");
		handleMM(myError);
		return 0;
	}

	for (uint32_t i = 0; i < NB_BUCKET; i++)
	{
		memset(&waveHdr[i], 0, sizeof(WAVEHDR));

		waveHdr[i].dwBufferLength = bucketSize;
		waveHdr[i].lpData = (char*)new uint8_t[bucketSize];

		if (waveOutPrepareHeader(myDevice, &waveHdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			printf("[Win32] waveOutPrepareHeader error\n");

		waveHdr[i].dwBufferLength = 0;
		waveHdr[i].dwFlags |= WHDR_DONE;
	}

	return 1;
}

uint8_t  win32AudioDevice::setVolume(int volume) 
{
	uint32_t value;

	value = (0xffff * volume) / 100;
	value = value + (value << 16);

	waveOutSetVolume(myDevice, value);

	return 1;
}

uint8_t win32AudioDevice::play(uint32_t len, float *data)
{
	if (len == 0)
		return 1;

	dither16(data, len, _channels);
	len *= 2;
	uint8_t success = 0;

	for (uint32_t i = 0; i < NB_BUCKET; i++)
	{
		if (waveHdr[i].dwFlags & WHDR_DONE)
		{
			waveHdr[i].dwFlags &= ~WHDR_DONE;

			if (len > bucketSize)
				waveHdr[i].dwBufferLength = bucketSize;
			else
				waveHdr[i].dwBufferLength = len;

			memcpy(waveHdr[i].lpData, data, waveHdr[i].dwBufferLength);
			data += waveHdr[i].dwBufferLength;
			len -= waveHdr[i].dwBufferLength;

			if (waveOutWrite(myDevice, &waveHdr[i], sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
				success = 1;
			else
				break;
		}

		if (len == 0)
			break;
	}

	if (len != 0)
	{
		printf("[Win32] No audio buffer available, %u bytes discarded\n", len);
		return 0;
	}

	return success;
}

void handleMM(MMRESULT err)
{
#define ERMM(x) if(err==x) printf("[Win32] "#x"\n");

	ERMM(MMSYSERR_ALLOCATED);
	ERMM(MMSYSERR_BADDEVICEID);
	ERMM(MMSYSERR_NODRIVER);
	ERMM(WAVERR_BADFORMAT);
	ERMM(WAVERR_SYNC);
}
#endif
