/***************************************************************************
                          TLK_filesel.cpp  -  description
                             -------------------
	New version of file selector

    begin                : Fri Sep 20 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#ifndef CYG_MANGLING
	#include <unistd.h>
#endif

#include "avi_vars.h"
#include "ADM_misc.h"
#include <ADM_assert.h>

#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit.hxx"
#include "prefs.h"
#define TH_READ 1
#define TH_WRITE 2
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"

extern char * actual_workbench_file;

static void GUI_FileSel(const char *label, SELFILE_CB * cb, int rw, char **name=NULL);
char            *PathCanonize(const char *tmpname);
void            PathStripName(char *str);
uint8_t         initFileSelector(void);

static GtkFileFilter   *filter_avi=NULL,*filter_mpeg=NULL,*filter_image=NULL,*filter_all=NULL;
static uint8_t          setFilter( GtkWidget *dialog);

/**
	Select a file
		Target is the string allocated by caller that will receive the resule, maxlen byte
		Source is a optionnal last file to replace the selector at the last dir used
	@Title@ is the title of the dialog window
	
	Returns : 0 if error, 1 on success
	

*/
uint8_t FileSel_SelectRead(const char *title,char *target,uint32_t max, const char *source)
{
	
GtkWidget *dialog;
uint8_t ret=0;
gchar *selected_filename;
gchar last;
char *dupe=NULL,*tmpname=NULL;
DIR *dir=NULL;
	
	dialog = gtk_file_chooser_dialog_new ("Open File",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                      NULL);
	gtk_window_set_title (GTK_WINDOW (dialog),title);
        initFileSelector();
        setFilter(dialog);
        gtk_register_dialog(dialog);
//	gtk_transient(dialog);
	if(source)
	{
		dupe=PathCanonize(source);
		PathStripName(dupe);
		if( (dir=opendir(dupe)) )
			{
				closedir(dir);
				gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),(gchar *)source);
			}
		delete [] dupe;
	
	}
	else	//use pref
	{
		if( prefs->get(LASTDIR_READ,(ADM_filename **)&tmpname))
		{
			
	
			dupe=PathCanonize(tmpname);
			PathStripName(dupe);

			if( (dir=opendir(dupe)) )
			{
				closedir(dir);
				gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),(gchar *)dupe);
			}
			delete [] dupe;
		}
	}
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
	{
			selected_filename= (gchar *) 	gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			if(strlen(selected_filename))
			{
			last=selected_filename[strlen(selected_filename) - 1]; 
			 if (last == '/' || last =='\\' )
			 {
                           GUI_Error_HIG(_("Cannot open directory as a file"), NULL);
						return 0;
			}
			else
			{
				// Check we can read it ..
				
					FILE *fd;
					fd=fopen(selected_filename,"rb");
					if(fd)
					{	
						fclose(fd);
						strncpy(target,(char *)selected_filename,max);
						// Finally we accept it :)
						ret=1;
					}
			}
			}
	}	
        gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	return ret;
}

void GUI_FileSelWrite(const char *label, SELFILE_CB * cb)
{				/* Create the selector */
    GUI_FileSel(label, cb, 1);
}

void GUI_FileSelRead(const char *label, SELFILE_CB * cb)
{				/* Create the selector */
    GUI_FileSel(label, cb, 0);
}
void GUI_FileSelRead(const char *label, char * * name)
{				/* Create the selector */
    GUI_FileSel(label, NULL, 0,name);
}
void GUI_FileSelWrite(const char *label, char * * name)
{				/* Create the selector */
    GUI_FileSel(label, NULL, 1,name);
}

// CYB 2005.02.23: DND
void fileReadWrite(SELFILE_CB *cb, int rw, char *name)
{

	if(name)
	{
		if(cb)
		{
			FILE *fd;
			fd=fopen(name,"rb");
			if(rw==0) // read
			{
				// try to open it..
				if(!fd)
				{
                                  GUI_Error_HIG(_("File error"), _("Cannot open \"%s\"."), name);
					return;
				}
			}
			else // write
			{
				if(fd){
				  struct stat buf;
				  int fdino;
					fclose(fd);
                                        if(!GUI_Question(_("Overwrite file ?")))
						return;
	                                /*
	                                ** JSC Fri Feb 10 00:07:30 CET 2006
	                                ** compare existing output file inode against each current open files inode
	                                ** i'm ignoring st_dev, so we may get false positives
	                                ** i'm testing until fd=1024, should be MAXFD computed by configure
	                                ** keep in mind:
	                                ** you can overwrite .idx files, they are loaded into memory and closed soon
	                                ** you cannot overwrite segment data files, all files are keeped open and
	                                **   are detected here
	                                */
#ifndef CYG_MANGLING
					if( stat(name,&buf) == -1 ){
						fprintf(stderr,"stat(%s) failed\n",name);
						return;
					}
#endif
					fdino = buf.st_ino;
					for(int i=0;i<1024;i++){
						if( fstat(i,&buf) != -1 ){
							if( buf.st_ino == fdino ){
							  char str[512];
								snprintf(str,512,"File \"%s\" exists and is opened by avidemux",name);
								GUI_Error_HIG(str,
                                                                    _("It could be possible that you try to overwrite an input file!"));
								return;
							}
						}
					}
					/*
	                                ** compare output file against actual EMCAscript file
					** need to stat() to avoid symlink (/home/x.js) vs. real file (/export/home/x.js) case
	                                */
					if( actual_workbench_file ){
						if( stat(actual_workbench_file,&buf) != -1 ){
							if( buf.st_ino == fdino ){
							  char str[512];
								snprintf(str,512,"File \"%s\" exists and is the actual ECMAscript file",name);
                                                                GUI_Error_HIG(str,_("It could be possible that you try to overwrite an input file!"));
								return;
							}
						}
					}
				}

				// check we have right access to it
				fd=fopen(name,"wb");
				if(!fd)
				{
                                  GUI_Error_HIG(_("Cannot write the file"),_( "No write access to \"%s\"."), name);
					return;
				}
			}
			fclose(fd);
			cb(name);
			ADM_dealloc(name);
		} // no callback -> return value
	}
}
// CYB 2005.02.23: DND


void GUI_FileSel(const char *label, SELFILE_CB * cb, int rw,char **rname)
{				/* Create the selector */

    	GtkWidget *dialog;
	char *name=NULL;
	char *tmpname;
	gchar *selected_filename;
        uint8_t res;

       
	if(rname)
		*rname=NULL;

        if(rw)
        {
                        dialog=dialog = gtk_file_chooser_dialog_new ("Save",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_SAVE,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                      NULL);
                        
        }
        else
        {
          dialog=dialog = gtk_file_chooser_dialog_new ("Open File",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                      NULL);
        }
        /**********/
        initFileSelector();
        setFilter(dialog);
        /**********/
        gtk_window_set_title (GTK_WINDOW (dialog),label);
        gtk_register_dialog(dialog);
        if(rw)
                res=prefs->get(LASTDIR_WRITE,(ADM_filename **)&tmpname); 
        else
               res=prefs->get(LASTDIR_READ,(ADM_filename **)&tmpname); 
        if(res)
	{
                DIR *dir;
                char *str=PathCanonize(tmpname);
                PathStripName(str);

                /* LASTDIR may have gone; then do nothing and use current dir instead (implied) */
                if( (dir=opendir(str)) )
                {
                        closedir(dir);
                        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),(gchar *)str);
                }
                delete [] str;
        }
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_ACCEPT)
	{

                        selected_filename= (gchar *) 	gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
#ifdef CYG_MANGLING
                        if (*(selected_filename + strlen(selected_filename) - 1) == '\\'){
#else			
                        if (*(selected_filename + strlen(selected_filename) - 1) == '/'){
#endif	 
                        GUI_Error_HIG(_("Cannot open directory as a file"), NULL);
                }
                else
                {
                        name=ADM_strdup(selected_filename);

                        char *str=PathCanonize(name);
                        PathStripName(str);
                        if(rw)
                                prefs->set(LASTDIR_WRITE,(ADM_filename *)str);			
                        else
                                prefs->set(LASTDIR_READ,(ADM_filename *)str);                        
                        delete [] str;

                }
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);

// CYB 2005.02.23
        if(cb)
        {
                fileReadWrite(cb, rw, name);
        }
        else
        {
                *rname=name;  
        }
}



/* Mean:It seems it is attached to the dialog & destroyed with it
   As it leads to crash if we don't recreate them each time....*/
uint8_t         initFileSelector(void)
{

#define ADD_PAT(x,y) gtk_file_filter_add_pattern(x,"*."#y);
        
        filter_avi=gtk_file_filter_new();
        gtk_file_filter_set_name(filter_avi,"Avi (*.avi)");
        ADD_PAT(filter_avi,avi);
        ADD_PAT(filter_avi,AVI);
        
        filter_mpeg=gtk_file_filter_new();
        gtk_file_filter_set_name(filter_mpeg,"Mpeg (*.m*,*.vob)");
        ADD_PAT(filter_mpeg,[mM][12][Vv]);
        ADD_PAT(filter_mpeg,[Mm][pP][gG]);
        ADD_PAT(filter_mpeg,[Vv][Oo][Bb]);
        ADD_PAT(filter_mpeg,ts);
        ADD_PAT(filter_mpeg,TS);

       
        filter_image=gtk_file_filter_new();
        gtk_file_filter_set_name(filter_image,_("Images"));
        ADD_PAT(filter_image,png);
        ADD_PAT(filter_image,bmp);
        ADD_PAT(filter_image,jpg);

        ADD_PAT(filter_image,PNG);
        ADD_PAT(filter_image,BMP);
        ADD_PAT(filter_image,JPG);


        
        filter_all=gtk_file_filter_new();
        gtk_file_filter_set_name(filter_all,_("All"));
        gtk_file_filter_add_pattern (filter_all, "*");

        return 1;
}

uint8_t setFilter( GtkWidget *dialog)
{
        gtk_file_chooser_add_filter     (GTK_FILE_CHOOSER(dialog), filter_all);
        gtk_file_chooser_add_filter     (GTK_FILE_CHOOSER(dialog), filter_avi);
        gtk_file_chooser_add_filter     (GTK_FILE_CHOOSER(dialog), filter_mpeg);
        gtk_file_chooser_add_filter     (GTK_FILE_CHOOSER(dialog), filter_image);
        
        return 1;
}
//EOF
