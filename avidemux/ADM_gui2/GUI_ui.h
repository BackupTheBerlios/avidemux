

void UI_updateFrameCount(uint32_t curFrame);
void UI_setFrameCount(uint32_t curFrame,uint32_t total);

void UI_updateTimeCount(uint32_t curFrame, uint32_t fps);
void UI_setTimeCount(uint32_t curFrame,uint32_t total, uint32_t fps);

double 	UI_readScale( void );
void 	UI_setScale( double  val );
void 	UI_setFrameType( uint32_t frametype,uint32_t qp);
void 	UI_setMarkers(uint32_t a, uint32_t b );
void 	UI_setTitle(char *name);


uint8_t UI_getPreviewToggleStatus( void );
uint8_t UI_setPreviewToggleStatus( uint8_t status );

void UI_setAProcessToggleStatus( uint8_t status );
void UI_setVProcessToggleStatus( uint8_t status );

void UI_iconify( void );
void UI_deiconify( void );

uint32_t UI_readCurFrame( void );
void UI_JumpDone(void);

