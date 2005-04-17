
#ifndef EQ2_PARAM
#define EQ2_PARAM
typedef struct Eq2_Param
{
  float     contrast ;      /* 1.0 means do nothing..*/    
  float     brightness;     /* 0 means */
  float     saturation;
  
  float     gamma;
  float     gamma_weight;
  float     rgamma;
  float     ggamma;
  float     bgamma;
}Eq2_Param;
uint8_t DIA_getEQ2Param(Eq2_Param *param);
#endif
