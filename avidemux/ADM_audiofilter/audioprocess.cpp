#include <stdio.h>
#include <assert.h>

#include "config.h"

#include "avifmt.h"
#include "audioprocess.hxx"

AVDMProcessAudioStream::AVDMProcessAudioStream(AVDMGenericAudioStream *instream){
	// we need a input stream !
	assert(instream);
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
	assert(0);
}

uint8_t AVDMProcessAudioStream::goToTime(uint32_t newoffset) {
    UNUSED_ARG(newoffset);
	assert(0);
}

// -------------------------------------------------------------

uint8_t AVDMBufferedAudioStream::goTo(uint32_t newoffset) {
	assert(!newoffset);
	goToTime(0);
	return 1;
}

uint8_t AVDMBufferedAudioStream::goToTime(uint32_t newoffset) {
	assert(!newoffset);
	_instream->goToTime(0);
	_headBuff=_tailBuff=0;
	return 1;
}
