
#ifndef DGBOB_PARAM
#define DGBOB_PARAM
typedef struct DGBobparam
{
        uint32_t  thresh;// low=more flickering, less jaggie
        uint32_t  order; //0 : Bottom field first, 1 top field first        
        uint32_t  mode;  // 0 keep # of frames, 1 *2 fps & *2 frame, 2  #*2, fps*150% slow motion
        uint32_t  ap;    // Extra artifact check, better not to use
}DGBobparam;

#endif
