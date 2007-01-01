//
// C++ Interface: ADM_compressedImage.h
//
// Description: 
//
//  Describe a compressed packet coming from the demuxer
//
//
#ifndef ADM_COMPRESSED_IMAGE_H
#define ADM_COMPRESSED_IMAGE_H

#define ADM_COMPRESSED_NO_PTS 0xffffffff
class ADMCompressedImage
{
  
  public :
    
        /* Our datas */
        uint8_t *data;
        uint32_t dataLength;
        /* Associated flags, in most cases filled by decoder */
        uint32_t flags;
        /* Some interesting informations */
        uint32_t demuxerFrameNo; /* In bitstream order i.e. decoding order */
        uint32_t demuxerPts;  /* In ms !*/
        uint32_t demuxerDts;  /* In ms */
        /*         */
        void cleanup(uint32_t demuxerNo) 
            {
              flags=0;
              demuxerFrameNo=demuxerNo;
              demuxerPts=ADM_COMPRESSED_NO_PTS;  
              demuxerDts=ADM_COMPRESSED_NO_PTS;
            }
  
};

#endif
