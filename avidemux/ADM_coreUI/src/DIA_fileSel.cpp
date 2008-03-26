/***************************************************************************
                          DIA_fileSel.cpp
  
  (C) Mean 2008 fixounet@free.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ADM_default.h"
#include "DIA_coreUI_internal.h"

static DIA_FILESEL_DESC_T *fileSelDescriptor=NULL;
/**
 * 	\fn DIA_fileSelInit
 *  \brief Hook the fileDialog
 */
uint8_t DIA_fileSelInit(DIA_FILESEL_DESC_T *d)
{
	fileSelDescriptor=d;
	fileSelDescriptor->fileInit();
	return 1;
}

// A bunch of boomerang functions
/**
 * \fn GUI_FileSelRead
 * \brief Select a file for reading, when ok the CB callback is called with the name as argument
 */	
void GUI_FileSelRead(const char *label,SELFILE_CB cb) 
{
	ADM_assert(fileSelDescriptor);
	return fileSelDescriptor->fileReadCb(label,cb);	
}
/**
 * \fn GUI_FileSelWrite
 * \brief Select a file for Writing, when ok the CB callback is called with the name as argument
 */	
void GUI_FileSelWrite(const char *label,SELFILE_CB cb) 
{
	ADM_assert(fileSelDescriptor);
	return fileSelDescriptor->fileWriteCb(label,cb);	
}
/**
 * \fn GUI_FileSelRead
 * \brief Select a file for reading, name is allocated with a copy of the name, null if fail.
 */	
void GUI_FileSelRead(const char *label,char * * name) 
{
	ADM_assert(fileSelDescriptor);
	return fileSelDescriptor->fileReadName(label,name);	
}
/**
 * \fn GUI_FileSelWrite
 * \brief Select a file for Writing, name is allocated with a copy of the name, null if fail.
 */	

void GUI_FileSelWrite(const char *label,char * * name) 
{
	ADM_assert(fileSelDescriptor);
	return fileSelDescriptor->fileWriteName(label,name);	
}
/**
 * \fn FileSel_SelectWrite
 * \brief Select a file for Writing, name is allocated with a copy of the name, null if fail.
 * @param title : [in] Title of the dialog box
 * @param target: [in/out] Buffer that will hold the name, must be at least max bytes big
 * @param max : [in] Max number of bytes the buffer will hold
 * @param source : [in] Initial value for the file, can be null
 */	
uint8_t FileSel_SelectWrite(const char *title,char *target,uint32_t max, const char *source)
{
	ADM_assert(fileSelDescriptor);
	return fileSelDescriptor->fileSelectWrite(title,target,max, source);
}
/**
 * \fn FileSel_SelectRead
 * \brief Select a file for Reading, name is allocated with a copy of the name, null if fail.
 * @param title : [in] Title of the dialog box
 * @param target: [in/out] Buffer that will hold the name, must be at least max bytes big
 * @param max : [in] Max number of bytes the buffer will hold
 * @param source : [in] Initial value for the file, can be null
 */	
uint8_t FileSel_SelectRead(const char *title,char *target,uint32_t max, const char *source)
{
	ADM_assert(fileSelDescriptor);
	return fileSelDescriptor->fileSelectRead(title,target,max, source);
}
/**
 * \fn FileSel_SelectDir
 * \brief Select a directory, name is allocated with a copy of the name, null if fail.
 * @param title : [in] Title of the dialog box
 * @param target: [in/out] Buffer that will hold the name, must be at least max bytes big
 * @param max : [in] Max number of bytes the buffer will hold
 * @param source : [in] Initial value for the file, can be null
 */	
uint8_t FileSel_SelectDir(const char *title,char *target,uint32_t max, const char *source)
{
	ADM_assert(fileSelDescriptor);
	return fileSelDescriptor->fileSelectDirectory(title,target,max, source);
}

// EOF
