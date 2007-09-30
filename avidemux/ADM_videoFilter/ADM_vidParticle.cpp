/***************************************************************************
                          ADM_vidParticle.cpp  -  detect particles (groups
                                                     of pixels)
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
 
#include "default.h"

#include <math.h>

#include "ADM_assert.h"
#include "fourcc.h"
#include "avio.hxx"
#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"

#include "ADM_filter/video_filters.h"

#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"

#include "ADM_vidParticle.h"
#include "DIA_flyDialog.h" // for MenuMapping

FILE * ADMVideoParticle::outfp = 0;
uint32_t ADMVideoParticle::last_frame_written = 0xffffffff;

static FILTER_PARAM particleParam =
{
    10,
    { "min_area", "max_area", "left_crop", "right_crop",          // 4
      "top_crop", "bottom_crop", "output_format", "output_file",  // + 4 = 8
      "camera_number", "debug"                                    // + 2 = 10
    }
};

SCRIPT_CREATE(particle_script,ADMVideoParticle,particleParam);

BUILD_CREATE(particle_create,ADMVideoParticle);

ADMVideoParticle::ADMVideoParticle (AVDMGenericVideoStream *in, CONFcouple *couples)
{
    printf ("ADMVideoParticle ctor (%p)\n", this);
    _in = in;
    memcpy(&_info, in->getInfo(), sizeof(_info));
    _info.encoding = 1;
    _uncompressed = new ADMImage(_in->getInfo()->width, _in->getInfo()->height);
    ADM_assert(_uncompressed);
    _param = new PARTICLE_PARAM;

    if (couples)
    {
        GET(min_area);
        GET(max_area);
        GET(left_crop);
        GET(right_crop);
        GET(top_crop);
        GET(bottom_crop);
        GET(output_format);

        GET(output_file);
        GET(camera_number);
        GET(debug);
    }
    else
    {
        _param->min_area = 5;
        _param->max_area = 50000;
        _param->left_crop = 0;
        _param->right_crop = 0;
        _param->top_crop = 0;
        _param->bottom_crop = 0;
        _param->output_format = OUTPUTFMT_FORMAT_NEW;
        // _param->output_file = ""; // implicit
        _param->camera_number = 1;
        _param->debug = 0;
    }
}

uint8_t	ADMVideoParticle::getCoupledConf (CONFcouple **couples)
{

    ADM_assert(_param);
    *couples = new CONFcouple(particleParam.nb);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
    CSET(min_area);
    CSET(max_area);
    CSET(left_crop);
    CSET(right_crop);
    CSET(top_crop);
    CSET(bottom_crop);
    CSET(output_format);
    CSET(output_file);
    CSET(camera_number);
    CSET(debug);

    return 1;

}

uint8_t ADMVideoParticle::configure(AVDMGenericVideoStream *in)
{
    diaMenuEntry tOutputFmt [] = {
        { OUTPUTFMT_FORMAT_NEW,
          QT_TR_NOOP("New format, preferred, good for Tracker3D"), NULL },
        { OUTPUTFMT_FORMAT_OLD,
          QT_TR_NOOP("Old format that Tracker3D can't read directly"), NULL },
    };

    diaElemMenu output_format
        (&(_param->output_format), QT_TR_NOOP("Output _Format:"),
         sizeof (tOutputFmt) / sizeof (diaMenuEntry), tOutputFmt);

    MenuMapping menu_mapping [] = {
        { "outputFormatMenu", my_offsetof (PARTICLE_PARAM, output_format),
          sizeof (tOutputFmt) / sizeof (diaMenuEntry), tOutputFmt },
    };

    uint8_t ret = DIA_particle (_in, _param, menu_mapping,
                                sizeof (menu_mapping) / sizeof (MenuMapping));
    if (ret == 1)
    {
        return ret;
    }
    else if (ret == 0) // 0 = cancel
    {
        return ret;
    }
    else
    {
        ADM_assert (ret == 255); // 255 = whizzy dialog not implemented
    }

    diaElemUInteger min_area
        (&(_param->min_area),
         QT_TR_NOOP("Mi_nimum area for a particle to be detected:"), 1, 0x7fffffff);
    diaElemUInteger max_area
        (&(_param->max_area),
         QT_TR_NOOP("Ma_ximum area for a particle to be detected:"), 1, 0x7fffffff);

    diaElemUInteger left_crop
        (&(_param->left_crop),
         QT_TR_NOOP("_Left side crop (ignore particles in):"), 0, 0x7fffffff);
    diaElemUInteger right_crop
        (&(_param->right_crop),
         QT_TR_NOOP("_Right side crop (ignore particles in):"), 0, 0x7fffffff);
    diaElemUInteger top_crop
        (&(_param->top_crop),
         QT_TR_NOOP("_Top crop (ignore particles in):"), 0, 0x7fffffff);
    diaElemUInteger bottom_crop
        (&(_param->bottom_crop),
         QT_TR_NOOP("_Bottom crop (ignore particles in):"), 0, 0x7fffffff);

    diaElemFile output_file
        (1, &(_param->output_file), QT_TR_NOOP("_Output File:"), "pts");

    diaElemUInteger camera_number
        (&(_param->camera_number), QT_TR_NOOP("_Camera number:"), 1, 0x7fffffff);

    diaElemUInteger debug(&(_param->debug), QT_TR_NOOP("_Debugging settings (bits):"),
                          0, 0x7fffffff);

    diaElem * elems[] = { &min_area, &max_area, &left_crop, &right_crop,
                          &top_crop, &bottom_crop, &output_format,
                          &output_file, &camera_number, &debug };

    ret = diaFactoryRun ("Particle Detection Configuration",
                         sizeof (elems) / sizeof (diaElem *), elems);
    return ret;
}

ADMVideoParticle::~ADMVideoParticle()
{
    printf ("ADMVideoParticle dtor (%p)\n", this);
    DELETE(_param);
    delete _uncompressed;
    _uncompressed = NULL;
}

char *ADMVideoParticle::printConf (void)
{
    const int CONF_LEN = 1024;
    static char conf[CONF_LEN];

    char * cptr = conf;

    cptr += snprintf (conf, CONF_LEN, "ParticleList: Area=%u..%u",
                      _param->min_area, _param->max_area);

    if (_param->left_crop || _param->right_crop || _param->top_crop
        || _param->bottom_crop)
    {
        cptr += snprintf (cptr, CONF_LEN - (cptr - conf),
                          ", Crop (L/R/T/B): %u/%u/%u/%u",
                          _param->left_crop, _param->right_crop,
                          _param->top_crop, _param->bottom_crop);
    }

    if (!_param->output_file.empty())
    {
        cptr += snprintf (cptr, CONF_LEN - (cptr - conf), ", %s Output to %s",
                          _param->output_format == OUTPUTFMT_FORMAT_OLD
                          ? "Old" : "New", _param->output_file.c_str());
    }

    cptr += snprintf (cptr, CONF_LEN - (cptr - conf), ", Camera # %u",
                      _param->camera_number);

    if (_param->debug)
        cptr += snprintf (cptr, CONF_LEN - (cptr - conf),
                          ", debug=0x%x", _param->debug);

    return conf;
}

uint8_t ADMVideoParticle::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                                ADMImage *data, uint32_t *flags)
{
    if (frame >= _info.nb_frames)
        return 0;

    if (_param->debug & 1)
        printf ("in ADMVideoParticle::getFrameNumberNoAlloc(%d, ...)\n", frame);

    if (!_in->getFrameNumberNoAlloc (frame, len, _uncompressed, flags))
        return 0;

    uint32_t planesize = _info.width * _info.height;
    uint32_t size = (planesize * 3) >> 1;
    *len = size;

    uint32_t real_frame = frame + _info.orgFrame;
    OutputFmt outfmt = static_cast <OutputFmt> (_param->output_format);

    if (!outfp && real_frame == 0 && !_param->output_file.empty())
    {
        printf ("Preparing to write particle list to %s\n",
                _param->output_file.c_str());

        outfp = fopen (_param->output_file.c_str(), "w");
        if (!outfp)
        {
            perror (_param->output_file.c_str());
        }
        else if (real_frame == 0) // ick
        {
            if (outfmt == OUTPUTFMT_FORMAT_NEW)
                fprintf (outfp, "%% Frame  Camera  X  Y  Area  Particle\n");
            else
                fprintf (outfp, "%% Particle  Area  X  Y  Frame\n");
        }
    }

    uint8_t ret = doParticle (_uncompressed, data, _in, real_frame,
                              (last_frame_written == real_frame) ? 0 : outfp,
                              _param, _info.width, _info.height);

    last_frame_written = real_frame;

    if (outfp && real_frame >= _info.nb_frames + _info.orgFrame - 1)
    {
        fclose (outfp);
        outfp = 0;
        printf ("Finished writing particle list to %s\n",
                _param->output_file.c_str());
    }

    return ret;
}

uint8_t
ADMVideoParticle::doParticle (ADMImage * image, ADMImage * data,
                              AVDMGenericVideoStream * in,
                              uint32_t real_frame,
                              FILE * do_outfp, PARTICLE_PARAM * param,
                              uint32_t width, uint32_t height)
{
    uint32_t debug = param->debug;

    uint32_t planesize = width * height;
    uint32_t uvplanesize = planesize >> 2;
    uint32_t size = planesize + (uvplanesize * 2);
			
    memset (UPLANE (data), 128, uvplanesize);
    memset (VPLANE (data), 128, uvplanesize);
    memset (YPLANE (data), 0, planesize);

    // HERE: we should erase anything outside the ROI (region of interest),
    // unless we decide to handle that some other way and/or place...

    uint8_t * imagePixels = YPLANE (image);
    ImageTool imtool (imagePixels, width, height, data);
    imtool.setDebug (debug);
    imtool.setMinArea (param->min_area);
    imtool.setMaxArea (param->max_area);
    imtool.setCropping (param->left_crop, param->right_crop,
                        param->top_crop, param->bottom_crop);
    static uint32_t particleNum = 0;

    OutputFmt outfmt = static_cast <OutputFmt> (param->output_format);
    uint32_t camera_number = param->camera_number;

    for (uint32_t y = param->top_crop; y < height - param->bottom_crop; y++)
    {
        uint8_t * pixelrow = imagePixels + (y * width);
        for (uint32_t x = param->left_crop; x < width - param->right_crop; x++)
        {
            if (imtool.goodPixel (pixelrow [x]))
            {
                if (imtool.autoOutline (x, y) == 0)
                    continue;

                uint32_t area = imtool.particleArea();
                float centroid_x = imtool.particleCentroidX();
                float centroid_y = imtool.particleCentroidY();

                if (debug & 0x01)
                    printf ("%d %d %.6f %.6f %d\n",
                            ++particleNum, area, centroid_x, centroid_y,
                            real_frame + 1);

                if (!do_outfp)
                    continue;

                if (outfmt == OUTPUTFMT_FORMAT_NEW)
                {
                    fprintf (do_outfp, "%d %d %.6f %.6f %d %d\n",
                             real_frame + 1, camera_number, centroid_x, centroid_y,
                             area, ++particleNum);
                }
                else
                {
                    fprintf (do_outfp, "%d %d %.6f %.6f %d\n",
                             ++particleNum, area, centroid_x, centroid_y,
                             real_frame + 1);
                }
            }
        }
    }

    data->copyInfo(image);
    return 1;
}	                           

//////////////////////////////////////////////////////////////////////////////

char ImageTool::directionLetters [] = "ESWN#?";

PixelOffset ImageTool::leftOffset [ImageTool::DIRECTION_COUNT] =
{
//     East         South        West         North
    {  0, -1 },  {  1,  0 },  {  0,  1 },  { -1,  0 }
};

// Interesting - aheadOffset [n] == leftOffset [(n + 1) % DIRECTION_COUNT]...
PixelOffset ImageTool::aheadOffset [ImageTool::DIRECTION_COUNT] =
{
//     East         South        West         North
    {  1,  0 },  {  0,  1 },  { -1,  0 },  {  0, -1 }
};

// If we're using 4-connectedness (current implementation), then the following
// would be treated as two independent particles; with 8-connectedness, this
// would be a single particle with some "holes" in it.

// . . . * * . . .
// . . * * * . . .
// . * * * . * . .
// . * * * . * * .
// * * * . * * * .
// * * . * * * * .
// . * . * * * . .
// . . . * . . . .


uint8_t ImageTool::autoOutline (uint32_t x, uint32_t y)
{
    pixelList.clear();

    if (debug & 0x02)
        printf ("autoOutline(%d,%d):", x, y);

    ADM_assert (goodPixel (x, y));

    uint32_t startingX = x;
    uint32_t startingY = y;

    TracingDirection direction = DIRECTION_COUNT; // deliberately invalid
    TracingDirection newDirection = DIRECTION_EAST;

    int8_t
        Lx, Ly,  // left
        Ax, Ay;  // ahead

#if 0
    uint32_t prevX;
    uint32_t prevY;
    TracingDirection startingDirection = direction;
#endif

    uint32_t pointCount = 0;
    uint32_t pointMax = my_w * my_h; // more than that and something is wrong

    do
    {
#if 0
        prevX = x;
        prevY = y;
#endif

        pixelList.push_back (PixelLoc (x, y));
        if (debug & 0x04)
            printf (" %c(%d,%d)%c", directionLetters [direction],
                    x, y, directionLetters [newDirection]);

        if (++pointCount >= pointMax)
        {
            printf ("\nUh oh!  Too many points!! (%d) (@(%d,%d), dir %c)\n",
                    pointCount, x, y, directionLetters [direction]);
            showStuff();
            break;
        }

        if (newDirection != direction)
        {
            direction = newDirection;
            Lx = leftOffset [direction].x;
            Ly = leftOffset [direction].y;
            Ax = aheadOffset [direction].x;
            Ay = aheadOffset [direction].y;
#define ARx (Ax + Rx)
#define ARy (Ay + Ry)
#define Rx  (-Lx)
#define Ry  (-Ly)
#define BRx (Bx + Rx)
#define BRy (By + Ry)
#define Bx  (-Ax)
#define By  (-Ay)
#define BLx (Bx + Lx)
#define BLy (By + Ly)
        }

        if (goodPixel (x + Lx, y + Ly))
        {
            // Turn left
            x += Lx;
            y += Ly;
            newDirection = TracingDirection ((direction + TURN_LEFT)
                                             % DIRECTION_COUNT);
        }
        else if (goodPixel (x + Ax, y + Ay))
        {
            // Continue in same direction
            x += Ax;
            y += Ay;
            // newDirection = direction;
        }
        else if (goodPixel (x + Rx, y + Ry))
        {
            // Turn right
            x += Rx;
            y += Ry;
            newDirection = TracingDirection ((direction + TURN_RIGHT)
                                             % DIRECTION_COUNT);
        }
#if 0
// if we turn this on, we probably ought to be looking at the AL pixel as
// well.  (And turn on the BR case, below?)

        else if (goodPixel (x + ARx, y + ARy))
        {
            // Slide down one pixel and continue
            do that!;
        }
#endif
        else if (goodPixel (x + Bx, y + By))
        {
            // Turn around (reverse direction)
            x += Bx;
            y += By;
            newDirection = TracingDirection ((direction + TURN_AROUND)
                                             % DIRECTION_COUNT);
        }
#if 0
        else if (goodPixel (x + BRx, y + BRy))
        {
            // Slide down one pixel and turn right
            do that (the slide)!;
            newDirection = TracingDirection ((direction + TURN_RIGHT)
                                             % DIRECTION_COUNT);
        }
#endif
        else
        {
            // Apparently there is nowhere to go.  This should happen only if
            // we're looking at an isolated pixel (a one-pixel particle).

            if (pixelList.size() > 1)
            {
                printf ("\nUh oh!  We're stuck, but it's not a lonely pixel!  "
                        "(@(%d,%d), dir %c)\n",
                        x, y, directionLetters [direction]);
                showStuff();
                ADM_assert (pixelList.size() > 1);
            }

            break; // we'd fall out anyway, but this is faster & more explicit
        }

    } while (x != startingX || y != startingY);

    if (debug & 0x02)
        printf (" done! %d points in all\n", pixelList.size());

    // HERE: if we moved this to below the loop that does the centroid, etc.,
    // then we could show that info and also the fill-in of the particle,
    // based on reading the output image (if it exists).

    if (debug & 0x08)
        showStuff();

    // We now do 4 things in one pass: 1. count the pixels in the particle
    // (thus computing the area, defined as the number of pixels in the
    // particle); 2. clear those pixels in the input image so that we don't
    // count the particle more than once; 3. compute the centroid (defined as
    // the point whose x is the average of all the x's in the particle, and
    // whose y is the average of all the y's in particle); 4. if there is an
    // output image, draw the particle in the output, with the outline
    // highlighted.

    my_particleArea = 0;
    my_particleCentroidX = 0;
    my_particleCentroidY = 0;

    uint32_t sumOfXs = 0;
    uint32_t sumOfYs = 0;
    uint32_t count = 0;

    PixelList::iterator plit = pixelList.begin();
    while (plit != pixelList.end())
    {
        uint32_t px = plit->x;
        uint32_t py = plit->y;

        ++plit;

        if (my_outImage)
        {
            outPixel (px, py) = 255;
#ifdef USE_COLOR_IN_OUTPUT
            outUPixel (px, py) = 0;
            outVPixel (px, py) = 0;
#endif
        }

        // We do the above (set up output pixels) first so we ensure that the
        // outline is highlighted even if the code below has already displayed
        // some of the pixels in the outline (due to direct or indirect
        // adjacency to other outline pixels).

        if (getPixel (px, py) == 0)
            continue;

        // If we're still here, then this pixel is part of the particle but
        // has not yet been counted or otherwise processed.  There may be
        // other horizontally adjacent pixels, as well, which we will also
        // process now, if they are present - this accomplishes "filling" the
        // particle outline (except that any pixels that were actually not
        // "on" in the particle will be ignored).

        // First, we process the pixel itself, and any pixels to the right of
        // it (if they are on).

        uint32_t startX = px;

        count++;
        sumOfXs += px;
        sumOfYs += py;
        getPixel (px, py) = 0; // erase to prevent being counted again

        while (goodPixel (++px, py))
        {
            count++;
            sumOfXs += px;
            sumOfYs += py;
            getPixel (px, py) = 0; // erase to prevent being counted again
            if (my_outImage)
                outPixel (px, py) = 128;
        }

        // Now, we process any pixels to the left of the original one (if they
        // are on).

        px = startX;
        while (goodPixel (--px, py))
        {
            count++;
            sumOfXs += px;
            sumOfYs += py;
            getPixel (px, py) = 0; // erase to prevent being counted again
            if (my_outImage)
                outPixel (px, py) = 128;
        }
    }

    my_particleArea = count;

    if (my_particleArea < my_minArea || my_particleArea > my_maxArea)
    {
        if (my_outImage)
        {
            // We need to erase the particle we just drew. :-(

            plit = pixelList.begin();
            while (plit != pixelList.end())
            {
                uint32_t px = plit->x;
                uint32_t py = plit->y;

                ++plit;

                // First, we erase the pixel itself, and any pixels to the right of
                // it (if they are on).

                uint32_t startX = px;

                while (validPixel (px, py) && outPixel (px, py))
                    outPixel (px++, py) = 0;

                // Now, we process any pixels to the left of the original one (if they
                // are on).

                px = startX;
                while (validPixel (--px, py) && outPixel (px, py))
                    outPixel (px, py) = 0;
            }
        }

        return 0;
    }

    my_particleCentroidX = float (sumOfXs) / count;
    my_particleCentroidY = float (sumOfYs) / count;

    if (my_outImage)
    {
        uint32_t px = static_cast <uint32_t> (nearbyintf (my_particleCentroidX));
        uint32_t py = static_cast <uint32_t> (nearbyintf (my_particleCentroidY));
        if (validPixel (px, py))
            outPixel (px, py) = 255;
        else
            printf ("################# Uh oh, about to stomp invalid centroid pixel (%d, %d)!!\n",
                    px, py);
#ifdef USE_COLOR_IN_OUTPUT
        outUPixel (px, py) = 255;
        outVPixel (px, py) = 255;
#endif
    }

    return 1;
}

void ImageTool::showStuff () const
{
    uint32_t minx = 0xffffffff;
    uint32_t miny = 0xffffffff;
    uint32_t maxx = 0;
    uint32_t maxy = 0;

    PixelList::const_iterator plit = pixelList.begin();
    while (plit != pixelList.end())
    {
        uint32_t px = plit->x;
        uint32_t py = plit->y;

        if (px < minx)
            minx = px;
        if (px > maxx)
            maxx = px;
        if (py < miny)
            miny = py;
        if (py > maxy)
            maxy = py;

        ++plit;
    }

    minx -= (minx > 0);
    miny -= (miny > 0);
    maxx += (maxx < my_w - 1);
    maxy += (maxy < my_h - 1);

    printf ("(%d..%d, %d..%d):\n", minx, maxx, miny, maxy);

    for (uint32_t py = miny; py <= maxy; py++)
    {
        printf ("    ");
        for (uint32_t px = minx; px <= maxx; px++)
        {
            if (goodPixel (px, py))
            {
                printf ("*");
#if 0
// This is now done in autoOutline() where it belongs (more so than here,
// anyway).
                outPixel (px, py) = 64;
#endif
            }
            else
                printf (".");
        }
        printf ("\n");
    }

    printf ("\nOutline (%d points):\n\n", pixelList.size());

    std::string blankLine (maxx - minx + 1, '.');
    std::vector <std::string> showOutline (maxy - miny + 1, blankLine);

    plit = pixelList.begin();
    while (plit != pixelList.end())
    {
        uint32_t px = plit->x;
        uint32_t py = plit->y;

        char & ch = showOutline [py - miny][px - minx];
        if (ch == '.')
            ch = '1';
        else if (ch == '9')
            ch = '+';
        else if (ch != '+')
            ++ch;

#if 0
// This is now done in autoOutline() where it belongs (more so than here,
// anyway).
        getPixel (px, py) = 0;
        outPixel (px, py) = 255;
        outUPixel (px, py) = 0;
        outVPixel (px, py) = 0;
#endif

        ++plit;
    }

    std::vector <std::string>::iterator oit = showOutline.begin();
    while (oit != showOutline.end())
        std::cout << *oit++ << "\n";

    std::cout << "\npixelList: " << pixelList << "\n\n";
}
