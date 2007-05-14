#ifndef FLY_HUE_H
#define FLY_HUE_H
class flyHue : public ADM_flyDialog
{
  
  public:
   Hue_Param  param;
  public:
   uint8_t    process(void);
   uint8_t    download(void);
   uint8_t    upload(void);
   uint8_t    update(void);
   flyHue (uint32_t width,uint32_t height,AVDMGenericVideoStream *in,
                                    void *canvas, void *slider) : ADM_flyDialog(width, height,in,canvas, slider,1) 
                    {
                      
                    };
};
#endif
