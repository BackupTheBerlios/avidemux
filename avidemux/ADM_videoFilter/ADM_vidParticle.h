/***************************************************************************
                          ADM_vidParticle.h  -  detect particles (groups of pixels)
                              -------------------
                          Chris MacGregor, 2005, 2007
                         chris-avidemux@bouncingdog.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef __PARTICLE__
#define __PARTICLE__   

#include <string>

#include "ADM_video/ADM_cache.h"

struct PARTICLE_PARAM
{
    uint32_t min_area;
    uint32_t max_area;
    uint32_t left_crop;
    uint32_t right_crop;
    uint32_t top_crop;
    uint32_t bottom_crop;
    uint32_t output_format;
    std::string output_file;
    uint32_t camera_number;
    uint32_t debug;
};

// Alas, because offsetof() is only supposed to work on POD (plain old data)
// structs, and our PARTICLE_PARAM includes a std::string (which has a
// constructor, and which causes PARTICLE_PARAM to therefore have an implicit
// constructor), we need to define our own offsetof() to use for the dialog
// menus.  See
// http://www.cplusplus.com/reference/clibrary/cstddef/offsetof.html for more
// on offsetof().

#define my_offsetof(_type, _memb) (size_t (&(((_type *)1)->_memb)) - 1)

class ADMVideoParticle : public AVDMGenericVideoStream
{

 protected:
    	
     PARTICLE_PARAM *  _param;

     static FILE * outfp; // ugly, but hard to avoid
     static uint32_t last_frame_written; // ditto

 public:
 		
     enum OutputFmt
     {
         OUTPUTFMT_INVALID = 0,

         OUTPUTFMT_FORMAT_NEW,
         OUTPUTFMT_FORMAT_OLD,

         OUTPUTFMT_COUNT
     };


     ADMVideoParticle (AVDMGenericVideoStream *in, CONFcouple *setup);

     ~ADMVideoParticle();

     virtual uint8_t getFrameNumberNoAlloc (uint32_t frame, uint32_t *len,
                                            ADMImage *data,uint32_t *flags);

     virtual uint8_t configure (AVDMGenericVideoStream *instream);
     virtual char * printConf (void);
     virtual uint8_t getCoupledConf (CONFcouple **couples);
							
     static uint8_t doParticle (ADMImage * image, ADMImage * data,
                                AVDMGenericVideoStream * in,
                                uint32_t real_frame,
                                FILE * do_outfp, PARTICLE_PARAM * param,
                                uint32_t width, uint32_t height);
};

//////////////////////////////////////////////////////////////////////////////

#include <list>
#include <vector>
#include <iostream>
#include <iterator>

class PixelLoc
{
public:
    uint16_t x;
    uint16_t y;

    PixelLoc ()
        : x (0),
          y (0)
    {
    }

    PixelLoc (uint32_t a_x, uint32_t a_y)
        : x (a_x),
          y (a_y)
    {
    }

    // Use this to sort by y's, then (for PixelLoc's with the same y) by x's.

    class Compare
    {
    public:
        int operator () (const PixelLoc & p1, const PixelLoc & p2) const
        {
            return ((p1.y == p2.y) ? (p1.x < p2.x) : (p1.y < p2.y));
        }
    };
};

// Now that we no longer need to sort the list or insert anything into it, a
// vector should be faster.
// typedef std::list <PixelLoc> PixelList;
typedef std::vector <PixelLoc> PixelList;

inline std::ostream &
operator << (std::ostream & s, const PixelLoc & pl)
{
    return s << "(" << pl.x << "," << pl.y << ")";
}

inline std::ostream &
operator << (std::ostream & s, const PixelList & theList)
{
    s << "[ ";
    std::copy (theList.begin(), theList.end(),
               std::ostream_iterator <PixelLoc> (s, " "));
    s << "]";
    return s;
}

struct PixelOffset
{
    int8_t x;
    int8_t y;
};

class ImageTool
{
private:
    uint8_t * my_pixels;
    uint32_t my_w;
    uint32_t my_h;
    uint32_t my_left_margin;
    uint32_t my_right_margin;
    uint32_t my_top_margin;
    uint32_t my_bottom_margin;
    ADMImage * my_outImage;
    uint32_t my_minArea;
    uint32_t my_maxArea;
    uint32_t my_particleArea;
    float my_particleCentroidX;
    float my_particleCentroidY;
    uint32_t debug;
    PixelList pixelList;

    enum TracingDirection
    {
        // The order of the members here is important - several things depend
        // on it!

        DIRECTION_EAST,   // x + 1, y
        DIRECTION_SOUTH,  // x    , y + 1
        DIRECTION_WEST,   // x - 1, y
        DIRECTION_NORTH,  // x    , y - 1
        DIRECTION_COUNT
    };

    enum TurnDirection
    {
        // These values are designed to be added to TracingDirection values.
        TURN_RIGHT = 1,
        TURN_AROUND = 2, // reverse
        TURN_LEFT = 3,
    };

    static char directionLetters [];
    static PixelOffset leftOffset [DIRECTION_COUNT];
    static PixelOffset aheadOffset [DIRECTION_COUNT];

public:
    ImageTool (uint8_t * pixels, uint32_t w, uint32_t h, ADMImage * outImage = 0)
        : my_pixels (pixels),
          my_w (w),
          my_h (h),
          my_left_margin (0),
          my_right_margin (w),
          my_top_margin (0),
          my_bottom_margin (h),
          my_outImage (outImage),
          my_particleArea (0),
          my_particleCentroidX (0),
          my_particleCentroidY (0),
          debug (0)
    {
    }

    void setCropping (uint32_t left_crop, uint32_t right_crop,
                      uint32_t top_crop, uint32_t bottom_crop)
    {
        my_left_margin = left_crop;
        my_right_margin = my_w - right_crop;
        my_top_margin = top_crop;
        my_bottom_margin = my_h - bottom_crop;
    }

    // Return true if the pixel is part of a particle.

    bool goodPixel (uint8_t pixel) const
    {
        // HERE: If we wanted to incorporate thresholding into this step, or look
        // for black particles on a white background, this function would be the
        // place to make the change.

        return (pixel != 0);
    }

    uint8_t & getPixel (uint32_t x, uint32_t y) const
    {
        return my_pixels [(y * my_w) + x];
    }

    uint8_t & getPixel (uint32_t index) const
    {
        return my_pixels [index];
    }

    // This one does bounds checking, and forces references "off the edge" to
    // the nearest valid pixel - it's useful for convolutions.  It does not
    // currently respect the crop settings, only because we know that this
    // function is used only when those aren't set.

    uint8_t & getPixelSafely (int32_t x, int32_t y) const
    {
        if (x < 0)
            x = 0;
        else if (x >= my_w)
            x = my_w - 1;

        if (y < 0)
            y = 0;
        else if (y >= my_h)
            y = my_h - 1;

        return my_pixels [(y * my_w) + x];
    }

    uint8_t & getPixelSafely (uint32_t x, uint32_t y) const
    {
        return getPixelSafely (static_cast <int32_t> (x),
                               static_cast <int32_t> (y));
    }

    uint8_t & outPixel (uint32_t x, uint32_t y) const
    {
        return YPLANE (my_outImage) [(y * my_w) + x];
    }

    uint8_t & outUPixel (uint32_t x, uint32_t y) const
    {
        return UPLANE (my_outImage) [((y >> 2) * my_w) + (x >> 1)];
    }

    uint8_t & outVPixel (uint32_t x, uint32_t y) const
    {
        return VPLANE (my_outImage) [((y >> 2) * my_w) + (x >> 1)];
    }

    bool validPixel (uint32_t x, uint32_t y) const
    {
        // return (x < my_w && y < my_h);
        return (x >= my_left_margin && x < my_right_margin
                && y >= my_top_margin && y < my_bottom_margin);
    }

    bool goodPixel (uint32_t x, uint32_t y) const
    {
        // HERE: this might be a good place to implement ROI (region of
        // interest) if we want that.

        return (validPixel (x, y) && goodPixel (getPixel (x, y)));
    }

    uint32_t width () const
    {
        return my_w;
    }

    uint32_t height () const
    {
        return my_h;
    }

    uint32_t particleArea () const
    {
        return my_particleArea;
    }

    float particleCentroidX () const
    {
        return my_particleCentroidX;
    }

    float particleCentroidY () const
    {
        return my_particleCentroidY;
    }

    uint8_t autoOutline (uint32_t x, uint32_t y);

    void showStuff () const;

    // implementation is in ADM_vidSwissArmyKnife.cpp - nothing else uses it
    // (currently).

    template <class Oper, class Histo>
    void convolve (const std::vector <float> & kernel,
                   uint32_t kw, uint32_t kh, int32_t bias, const Oper & op,
                   const Histo & histogram_in);

    void setDebug (uint32_t newDebug)
    {
        debug = newDebug;
    }

    void setMinArea (uint32_t newMinArea)
    {
        my_minArea = newMinArea;
    }

    void setMaxArea (uint32_t newMaxArea)
    {
        my_maxArea = newMaxArea;
    }
};

struct MenuMapping;
uint8_t DIA_particle (AVDMGenericVideoStream *in,
                      PARTICLE_PARAM * param,
                      const MenuMapping * menu_mapping,
                      uint32_t menu_mapping_count);

#endif
