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
//	This is the base time for image exchanged between codec/filters/...
//
//
#ifndef ADM_IMAGE
#define ADM_IMAGE

typedef enum ADM_ASPECT
{
	ADM_ASPECT_4_3=1,
	ADM_ASPECT_16_9,
	ADM_ASPECT_1_1
};
class ADMImage
{
public:
	uint8_t		*data;		/// Pointer to actual image data
	uint32_t	_width;		/// Width of image
	uint32_t	_height;	/// Height of image
	uint32_t	_qStride;	/// Stride of Q infos, usually about width/8
	uint8_t		*quant;		/// Byte representing quantize used for this block
	uint32_t	_Qp;		/// Average quantizer for this image, Default=2
	uint32_t	_qSize;		/// Size of the *quant bitfield
	ADM_ASPECT	_aspect;	/// Aspect ratio
	uint32_t	flags;		/// Flags for this image (AVI_KEY_FRAME/AVI_B_FRAME)
	
public:
		ADMImage(uint32_t width, uint32_t height);
		~ADMImage();
	uint8_t duplicate(ADMImage *src);	/// copy an image to ourself


};
#define YPLANE(x) (x->data)
#define UPLANE(x) (x->data+(x->_width*x->_height))
#define VPLANE(x) (x->data+(5*(x->_width*x->_height)>>2))
#endif
