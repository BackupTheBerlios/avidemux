//
typedef void SELFILE_CB(char *);
void GUI_FileSelRead(const char *label,SELFILE_CB cb) ;
void GUI_FileSelWrite(const char *label,SELFILE_CB cb) ;
void GUI_FileSelRead(const char *label, char * * name);
void GUI_FileSelWrite(const char *label, char * * name);

uint8_t FileSel_SelectRead(const char *title,char *target,uint32_t max, const char *source);
