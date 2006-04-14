#include <stdio.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"

#include "avifmt.h"
#include "audioprocess.hxx"

AVDMProcessAudioStream::AVDMProcessAudioStream(AVDMGenericAudioStream *instream){
	// we need a input stream !
	ADM_assert(instream);
	// store init parameters
	_instream = instream;
}

uint8_t AVDMProcessAudioStream::preprocess( void ) {
	return 1;
}

uint8_t AVDMProcessAudioStream::configure( void )  {
	return 1;
}

uint8_t AVDMProcessAudioStream::goTo(uint32_t newoffset) {
    UNUSED_ARG(newoffset);
	ADM_assert(0);
}

uint8_t AVDMProcessAudioStream::goToTime(uint32_t newoffset) {
    UNUSED_ARG(newoffset);
	ADM_assert(0);
}

// -------------------------------------------------------------

uint8_t AVDMBufferedAudioStream::goTo(uint32_t newoffset) {
	ADM_assert(!newoffset);
	goToTime(0);
	return 1;
}

uint8_t AVDMBufferedAudioStream::goToTime(uint32_t newoffset) {
	ADM_assert(!newoffset);
	_instream->goToTime(0);
	_headBuff=_tailBuff=0;
	return 1;
}
