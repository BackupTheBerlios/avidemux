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
#ifndef EDITOR_CACHE__
#define EDITOR_CACHE__
#include "ADM_library/ADM_image.h"
typedef struct cacheElem
{
	uint32_t lastUse;
	uint32_t frameNum;
	ADMImage *image;

}cacheElem;
class EditorCache
{
	private :
			uint32_t	_counter;
			cacheElem	 *_elem;
			uint32_t	_nbImage;
	public:
					EditorCache(uint32_t size,uint32_t w, uint32_t h);
					~EditorCache(void);
			ADMImage	*getFreeImage(void);	
			ADMImage 	*getImage(uint32_t no);
			uint8_t		updateFrameNum(ADMImage *image,uint32_t frame);

};
#endif
