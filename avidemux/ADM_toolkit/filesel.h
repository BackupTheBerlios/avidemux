//
typedef void SELFILE_CB(char *);
void GUI_FileSelRead(const char *label,SELFILE_CB cb) ;
void GUI_FileSelWrite(const char *label,SELFILE_CB cb) ;
void GUI_FileSelRead(const char *label, char * * name);
void GUI_FileSelWrite(const char *label, char * * name);


