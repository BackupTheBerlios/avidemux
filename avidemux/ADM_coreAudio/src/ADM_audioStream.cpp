/**
    \file ADM_audioStream
    \brief Base class

*/
#include "ADM_default.h"
#include "ADM_audioStream.h"


/**
    \fn ADM_audioStream
    \brief constructor
*/
ADM_audioStream::ADM_audioStream(WAVHeader *header,ADM_audioAccess *access)
{
    if(header)
        wavHeader=*header;
    else    
        memset(&wavHeader,0,sizeof(wavHeader));
    this->access=access;
    lastDts=ADM_AUDIO_NO_DTS;
}
/**
    \fn goToTime
    \brief goToTime
*/
uint8_t         ADM_audioStream::goToTime(uint64_t nbUs)
{
    if(access->canSeekTime()==true)
    {
        if( access->goToTime(nbUs)==true)
        {
           lastDts=nbUs;
           return 1;
        }
        return 1;
    }
    ADM_assert(true==access->canSeekOffset());
    // Convert time to offset in bytes
    float f=nbUs*wavHeader.byterate;
    f/=f/1000;
    f/=f/1000; // in bytes
    if(access->setPos( (uint32_t)(f+0.5)))
    {
        lastDts=nbUs;
        return 1;
    }
    return false;
}
/**
        \fn getPacket
*/
uint8_t ADM_audioStream::getPacket(uint8_t *buffer,uint32_t *size, uint32_t sizeMax,uint32_t *nbSample)
{
uint64_t dts=0;
    if(!access->getPacket(buffer,size,sizeMax,&dts)) return 0;
    // We got the packet
    // Try to guess the nbSample
    if(lastDts==ADM_AUDIO_NO_DTS)
    {
        *nbSample=512;
        printf("[audioStream] Cant guess nb sample, setting 512\n");
        return 1;
    }
    float f=dts-lastDts;
    f*=wavHeader.frequency;
    f/=1000;
    f/=1000;
    lastDts=dts;
    *nbSample=(uint32_t)(f+0.5);

}
/**
        \fn getExtraData
*/

bool         ADM_audioStream::getExtraData(uint32_t *l, uint8_t **d)
{
    return access->getExtraData(l,d);
}
/**
                Create the appropriate audio stream
*/
ADM_audioStream  *ADM_audioCreateStream(WAVHeader *wavheader, ADM_audioAccess *access)
{
uint8_t *data;
uint32_t size;
    return new ADM_audioStream(wavheader,access);

}
// EOF
