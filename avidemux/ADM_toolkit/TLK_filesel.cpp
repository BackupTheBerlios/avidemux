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
#include <ADM_assert.h>
#include <gtk/gtk.h>

#include <dirent.h>
#include <errno.h>
#ifndef CYG_MANGLING
	#include <unistd.h>
#endif

#include "avi_vars.h"
#include "toolkit.hxx"
#include "filesel.h"
#include "prefs.h"
#define TH_READ 1
#define TH_WRITE 2
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit_gtk.h"

static GtkWidget	*create_fileselection1 (void);
static void GUI_FileSel(const char *label, SELFILE_CB * cb, int rw, char **name=NULL);

char *PathCanonize(const char *tmpname);
void		PathStripName(char *str);



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

#if 1 || (GTK_MINOR_VERSION*10+GTK_MICRO_VERSION)<34

void GUI_FileSel(const char *label, SELFILE_CB * cb, int rw,char **rname)
{				/* Create the selector */

    	GtkWidget *dialog;
	char *name=NULL;
	char *tmpname;
	gchar *selected_filename;

	if(rname)
		*rname=NULL;

	dialog=create_fileselection1();
	gtk_window_set_title (GTK_WINDOW (dialog),label);
	gtk_transient(dialog);
	if( prefs->get(LASTDIR,&tmpname))
	{
		DIR *dir;
	//	printf("tmpname : %s\n",tmpname);
		char *str=PathCanonize(tmpname);

	//	printf("tmpname : %s\n",str);
		PathStripName(str);

	//	printf("tmpname : *%s*\n",str);
		/* LASTDIR may have gone; then do nothing and use current dir instead (implied) */
		if( (dir=opendir(str)) ){
			closedir(dir);
			gtk_file_selection_set_filename(GTK_FILE_SELECTION(dialog),(gchar *)str);
		}
		delete [] str;
	}
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{

			selected_filename= (gchar *) 	gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog));
			 if (*(selected_filename + strlen(selected_filename) - 1) == '/'){
      	  					GUI_Alert("Cannot open directory as file !");
			}
			else
			{
						name=strdup(selected_filename);

						char *str=PathCanonize(name);
						PathStripName(str);
						prefs->set(LASTDIR,str);			
						delete [] str;

			}
	}
	gtk_widget_destroy(dialog);

	if(name)
	{

		if(cb)
		{

			if(rw==0) // read
			{
				// try to open it..
				FILE *fd;
				fd=fopen(name,"rb");
				if(!fd)
				{
						GUI_Alert("Cannot open this file !");
						return;
				}
				fclose(fd);
				cb(name);
				free(name);

			}
			else // write
			{
				FILE *fd;
				fd=fopen(name,"rb");
				if(fd)
					{
							fclose(fd);
							if(!GUI_Question("Overwrite file ?"))
								return;
					}
				// check we have right access to it
				fd=fopen(name,"wb");
				if(!fd)
					{
						GUI_Alert("No write access to that file !");
						return;
					}
				fclose(fd);


				cb(name);
				free(name);
			}
		} // no callback -> return value
		else
		{

			*rname=name;


		}


	}

}
#else
//
//	Gtk 2.4 Using fileChooser instead of fileselect
//
void GUI_FileSel(const char *label, SELFILE_CB * cb, int rw,char **rname)
{				/* Create the selector */

    	GtkWidget *dialog;
	char *name=NULL;
	char *tmpname;
	gchar *selected_filename;
	GtkFileChooserAction action;
	
	if(rname)
		*rname=NULL;

	if(!rw) 	action=GTK_FILE_CHOOSER_ACTION_OPEN;
		else 	action=GTK_FILE_CHOOSER_ACTION_SAVE;

	dialog = gtk_file_chooser_dialog_new (label,
				      NULL,
				      action,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      NULL);
				      
	if( prefs->get(LASTDIR,&tmpname))
	{
		char *str=PathCanonize(tmpname);
		PathStripName(str);
		
		printf("Current folder : *%s*\n",str);
		if(str)
		{	// remove last /
			str[strlen(str)-1]=0;
	 		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(dialog), str);
		}
		delete [] str;
	}				      
	
			      
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
  	{
    		selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));    
  	
	 if (*(selected_filename + strlen(selected_filename) - 1) == '/')
      	  					GUI_Alert("Cannot open directory as file !");
			else
			{
						name=strdup(selected_filename);

						char *str=PathCanonize(name);
						PathStripName(str);
						prefs->set(LASTDIR,str);			
						delete [] str;

	}		}
	gtk_widget_destroy (dialog);
	
	if(name)
	{

		if(cb)
		{

			if(rw==0) // read
			{
				// try to open it..
				FILE *fd;
				fd=fopen(name,"rb");
				if(!fd)
				{
						GUI_Alert("Cannot open this file !");
						return;
				}
				fclose(fd);
				cb(name);
				free(name);

			}
			else // write
			{
				FILE *fd;
				fd=fopen(name,"rb");
				if(fd)
					{
							fclose(fd);
							if(!GUI_Question("Overwrite file ?"))
								return;
					}
				// check we have right access to it
				fd=fopen(name,"wb");
				if(!fd)
					{
						GUI_Alert("No write access to that file !");
						return;
					}
				fclose(fd);


				cb(name);
				free(name);
			}
		} // no callback -> return value
		else
		{

			*rname=name;


		}


	}

}

#endif



//------------------------------------------------------------------

static gint response_accept_cb(GtkWidget *widget,  gpointer  *data);
static gint response_cancel_cb(GtkWidget *widget,  gpointer  *data);

GtkWidget	*create_fileselection1 (void)
{
  GtkWidget *fileselection1;
  GtkWidget *ok_button1;
  GtkWidget *cancel_button1;

  fileselection1 = gtk_file_selection_new (_("Select File"));
    gtk_widget_show (fileselection1);
  gtk_container_set_border_width (GTK_CONTAINER (fileselection1), 10);

  ok_button1 = GTK_FILE_SELECTION (fileselection1)->ok_button;
  gtk_widget_show (ok_button1);
  GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);

  cancel_button1 = GTK_FILE_SELECTION (fileselection1)->cancel_button;
  gtk_widget_show (cancel_button1);
  GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);
// Patch
g_signal_connect (    	GTK_OBJECT (ok_button1), "clicked", 	G_CALLBACK (response_accept_cb), fileselection1);
g_signal_connect (    	GTK_OBJECT (cancel_button1), "clicked", 	G_CALLBACK (response_cancel_cb), fileselection1);

// /Patch
  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (fileselection1, fileselection1, "fileselection1");
  GLADE_HOOKUP_OBJECT_NO_REF (fileselection1, ok_button1, "ok_button1");
  GLADE_HOOKUP_OBJECT_NO_REF (fileselection1, cancel_button1, "cancel_button1");

  return fileselection1;
}
gint response_accept_cb(GtkWidget *widget,  gpointer  *data)
{
UNUSED_ARG(widget);
    GtkWidget  *dialog = (GtkWidget *) data;
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    return TRUE;
}
gint response_cancel_cb(GtkWidget *widget,  gpointer  *data)
{
UNUSED_ARG(widget);
    GtkWidget  *dialog = (GtkWidget *) data;
    gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    return TRUE;
}

/*
** note: it modifies it's first argument
*/
void simplify_path(char **buf){
   unsigned int last1slash = 0;
   unsigned int last2slash = 0;
	while( !strncmp(*buf,"/../",4) )
		memmove(*buf,*buf+3,strlen(*buf+3)+1);
	for(unsigned int i=0;i<strlen(*buf)-2;i++)
		while( !strncmp(*buf+i,"/./",3) )
			memmove(*buf+i,*buf+i+2,strlen(*buf+i+2)+1);
	for(unsigned int i=0;i<strlen(*buf)-3;i++){
		if( *(*buf+i) == '/' ){
			last2slash = last1slash;
			last1slash = i;
		}
		if( !strncmp(*buf+i,"/../",4) ){
			memmove(*buf+last2slash,*buf+i+3,strlen(*buf+i+3)+1);
			return simplify_path(buf);
		}
	}
}

//
//	Make it absolute
//
char *PathCanonize(const char *tmpname)
{
	char path[300];
	char *out ;

	if( ! getcwd(path,300) ){
		fprintf(stderr,"\ngetcwd() failed with: %s (%u)\n",
		               strerror(errno), errno );
		path[0] = '\0';
	}
	if(!tmpname || tmpname[0]==0)
	{
		out=new char [strlen(path)+2];
		strcpy(out,path);
		strcat(out,"/");
		printf("\n Canonizing null string ??? (%s)\n",out);
	}else if(tmpname[0]=='/'
#if defined(CYG_MANGLING)
		|| tmpname[1]==':'
#endif	
	
	)
	{
		out=new char[strlen(tmpname)+1];
		strcpy(out,tmpname);
		return out;
	}else{
		out=new char[strlen(path)+strlen(tmpname)+2];
		strcpy(out,path);
		strcat(out,"/");
		strcat(out,tmpname);
	}
	simplify_path(&out);
	return out;
}
/*
	Strip the path and only keep the name

*/
void		PathStripName(char *str)
{
		int len=strlen(str);
		if(len<=1) return;
		len--;
		while( *(str+len)!='/' && len)
		{
			 *(str+len)=0;
			 len--;
		}
}
/*
	Split patch into absolute path+name and extention

*/
void PathSplit(char *str, char **root, char **ext)
{
	char *full;
	uint32_t l;

		full=PathCanonize(str);
		// Search the last .
		l=strlen(full);
		l--;
		ADM_assert(l>0);
		while( *(full+l)!='.' && l) l--;
		if(!l || l==(strlen(full)-1))
		{
			if(l==(strlen(full)-1))
			{
				*(full+l)=0;  // remove trailing .
			}
			*ext=new char[2];
			*root=full;
			strcpy(*ext,"");
			return ;
		}
		// else we do get an extension
		// starting at l+1
		uint32_t suff;

		suff=strlen(full)-l-1;
		*ext=new char[suff+1];
		strcpy(*ext,full+l+1);
		*(full+l)=0;
		*root=full;
		return ;
}
