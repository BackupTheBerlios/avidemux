#if 0 //ndef ADM_VODEC_L
#define ADM_VODEC_L

typedef struct codecEnumByName
{
        SelectCodecType type;
        const char      *displayName;
        const char      *name;
}codecEnumByName;

static const codecEnumByName mycodec[]=
{
        {CodecCopy      ,"Copy","Copy"},
#ifdef USE_DIVX
        {CodecDivx      ,"Divx","Divx"},
#endif
#ifdef USE_XX_XVID
        {CodecXvid      ,"Xvid","Xvid"},
#endif

#ifdef USE_XVID_4
        {CodecXvid4     ,"Xvid4","Xvid4"},
#endif
        {CodecMjpeg     ,"MJpeg","Mjpeg"},
#ifdef USE_MJPEG
        {CodecVCD       ,"VCD","VCD"},
        {CodecSVCD      ,"SVCD","SVCD"},
        {CodecDVD       ,"DVD","DVD"},
#endif
        {CodecXVCD      ,"XVCD","XVCD"},
        {CodecXSVCD     ,"SVCD (lavc)","XSVCD"},
        {CodecXDVD      ,"DVD (lavc)","XDVD"},
        {CodecFF        ,"Lav Mpeg4","FFmpeg4"},
        {CodecH263      ,"H263","H263"},
        {CodecFFhuff    ,"FF Huffyuv","FFHUFF"},
        {CodecH263P     ,"H263+","H263+"},
        {CodecHuff      ,"Huffyuv","Huffyuv"},
        {CodecFFV1      ,"FFV1","FFV1"},
#ifdef USE_X264
        {CodecX264,     "X264","X264"},
#endif                
        {CodecSnow      ,"Snow","Snow"}
};


#endif
