/***************************************************************************
                          ADM_vidEncode.hxx  -  description
                             -------------------
    begin                : Sun May 5 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #ifndef ADM_VIDENCODE
 #define ADM_VIDENCORE
 typedef enum
{
  COMPRESS_CQ,
  COMPRESS_CBR,
  COMPRESS_2PASS
}COMPRESSION_MODE;



typedef struct
{
    COMPRESSION_MODE 	mode;
    uint32_t					qz,bitrate,finalsize;

}COMPRES_PARAMS;

#endif
