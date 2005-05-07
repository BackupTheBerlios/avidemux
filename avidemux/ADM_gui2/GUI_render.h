/*


*/



uint8_t renderInit( void );
uint8_t renderResize(uint32_t w, uint32_t h);
uint8_t renderRefresh(void);
uint8_t renderExpose(void);
uint8_t renderUpdateImage(uint8_t *ptr);

uint8_t renderStartPlaying( void );
uint8_t renderStopPlaying( void );


typedef enum ADM_RENDER_TYPE
{
        RENDER_GTK=0,
#ifdef USE_XV
        RENDER_XV=1,
#endif
#ifdef USE_SDL
        RENDER_SDL=2,
#endif
        RENDER_LAST       


};

