//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __X264_PARM_
#define __X264_PARM_
typedef struct ADM_x264Param
{
  
  uint32_t qmin;
  uint32_t qmax;
  uint32_t cabac;
  uint32_t maxKf;
  uint32_t minKf;
  uint32_t nbBframe;  
  uint32_t globalHeader;
    
}ADM_x264Param;

#endif
