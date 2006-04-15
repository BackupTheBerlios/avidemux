/*


*/
#ifndef AUDIO_DEF
#define AUDIO_DEF
#include <string.h>
typedef struct
{
    uint16_t	encoding;	
    uint16_t	channels;					/* 1 = mono, 2 = stereo */
    uint32_t	frequency;				/* One of 11025, 22050, or 44100 48000 Hz */
    uint32_t	byterate;					/* Average bytes per second */
    uint16_t	blockalign;				/* Bytes per sample block */
    uint16_t	bitspersample;		/* One of 8, 12, 16, or 4 for ADPCM */
} WAVHeader;

typedef enum CHANNEL_CONF
{
    CHANNEL_INVALID=0,
    CHANNEL_MONO,
    CHANNEL_STEREO,
    CHANNEL_2F_1R,
    CHANNEL_3F,         //4
    CHANNEL_3F_1R,  
    CHANNEL_2F_2R,
    CHANNEL_3F_2R,
    CHANNEL_3F_2R_LFE, // 8
    CHANNEL_DOLBY_SURROUND,  //9
    CHANNEL_DOLBY_PROLOGIC,
    CHANNEL_DOLBY_PROLOGIC2, // 11
    CHANNEL_LAST
};
const int ADM_channel_mixer[12]=
{
    0,1,2,3,
    3,4,4,5,
    6,2,2,2
};
class ADM_ChannelMatrix
{
    public:
        uint32_t        nbChannel;
        CHANNEL_CONF    channelConfiguration;
        uint8_t         channelLayout[8];
        
        ADM_ChannelMatrix(void) {memset(this,0,sizeof(ADM_ChannelMatrix));}
};
#endif
