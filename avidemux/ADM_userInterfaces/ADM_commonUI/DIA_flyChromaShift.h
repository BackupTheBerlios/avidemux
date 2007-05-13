#ifndef FLY_CHROMASHIFT_H
#define FLY_CHROMASHIFT_H
class flyChromaShift : public ADM_flyDialog
{
  
  public:
   CHROMASHIFT_PARAM  param;
  public:
   uint8_t    process(void);
   uint8_t    download(void);
   uint8_t    upload(void);
   uint8_t    update(void);
   flyChromaShift (uint32_t width,uint32_t height,AVDMGenericVideoStream *in,
                                    void *canvas, void *slider) : ADM_flyDialog(width, height,in,canvas, slider,1) 
                    {
                      
                    };
};

#endif
//EOF
