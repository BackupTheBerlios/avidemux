/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <iconv.h>

#ifdef USE_FREETYPE

#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_video/ADM_vidASS.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

#define LABEL(txt) gtk_label_new(_(txt))

typedef struct {
	ASSParams *params;
	GtkLabel *label;
} FileSelCbData;

void cb_clicked(GtkButton *btn, void *data) {
	gtk_widget_set_sensitive((GtkWidget*)data, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btn)));
}

bool fileselection(const char *title, ADM_filename **filename, GtkFileChooserAction act = GTK_FILE_CHOOSER_ACTION_OPEN) {
	GtkWidget *dialog = gtk_file_chooser_dialog_new(_(title),
		NULL, act,
		GTK_STOCK_CANCEL, 0,
		GTK_STOCK_OPEN, 1, NULL);
	
	gtk_register_dialog(dialog);
	
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), (char*)*filename);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == 1) {
		char *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		
		gtk_unregister_dialog(dialog);
		gtk_widget_destroy(dialog);
		
		if(act == GTK_FILE_CHOOSER_ACTION_OPEN || act == GTK_FILE_CHOOSER_ACTION_SAVE) {
			int fd = open(fn, O_RDONLY);
			if(fd == -1) {
				GUI_Error_HIG(_("Can't read selected file"), NULL);
				
				return 0;
			}
			close(fd);
		}
		else {
			DIR *dir = opendir(fn);
			if(dir == NULL) {
				GUI_Error_HIG(_("Can't open selected directory"), NULL);
				return 0;
			}
			closedir(dir);
		}
		
		// ADM_dealloc can't free memory allocated by glib :(
		*filename = (ADM_filename*)ADM_alloc(sizeof(ADM_filename) * (strlen(fn)+1));
		strcpy((char*)*filename, fn);
		g_free(fn);
		
		return 1;
	}
	else {
		gtk_unregister_dialog(dialog);
		gtk_widget_destroy(dialog);
		
		return 0;
	}
}	

void btn_fd_clicked(GtkButton *btn, void *_data) {
	FileSelCbData *data = (FileSelCbData*)_data;
	
	char *oldpath = (char*)data->params->fonts_dir;
	if(fileselection("Embedded fonts directory", &data->params->fonts_dir, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER)) {
		ADM_dealloc(oldpath);
	}
	
	gtk_label_set_text(data->label, (char*)data->params->fonts_dir);
}

void btn_sub_clicked(GtkButton *btn, void *_data) {
	FileSelCbData *data = (FileSelCbData*)_data;
	
	char *oldpath = (char*)data->params->subfile;
	if(fileselection("Subtitle files", &data->params->subfile)) {
		ADM_dealloc(oldpath);
	}
	
	gtk_label_set_text(data->label, (char*)data->params->subfile);
}

uint8_t ADMVideoSubASS::configure(AVDMGenericVideoStream *instream) {
	GtkWidget *dialog, *vbox, *tbl, *e_fs, *e_ls, *e_tm, *e_bm, *cb, 
		*lbl_sub, *btn_sub, *hb_sub, *lbl_fd, *btn_fd, *hb_fd;
	int resp;
	FileSelCbData fs_sub, fs_fonts_dir;
	
	dialog = gtk_dialog_new_with_buttons("ASS subtitles configuration", NULL,
		(GtkDialogFlags)0, GTK_STOCK_CANCEL, 0, GTK_STOCK_OK, 1, NULL);
	vbox = GTK_DIALOG(dialog)->vbox;
	lbl_sub = gtk_label_new("");
	lbl_fd = gtk_label_new("");
	hb_sub = gtk_hbox_new(TRUE, 0);
	hb_fd = gtk_hbox_new(TRUE, 0);
	btn_sub = gtk_button_new_from_stock(GTK_STOCK_OPEN);
	btn_fd = gtk_button_new_from_stock(GTK_STOCK_OPEN);
	tbl = gtk_table_new(4, 2, TRUE);
	e_fs = gtk_spin_button_new_with_range(0, 10, 0.01);
	e_ls = gtk_spin_button_new_with_range(0, 10, 0.01);
	e_tm = gtk_spin_button_new_with_range(0, _info.height/2, 1);
	e_bm = gtk_spin_button_new_with_range(0, _info.height/2, 1);
	cb = gtk_check_button_new_with_label(_("Extract embedded fonts to"));
	
	fs_sub.params = fs_fonts_dir.params = _params;
	fs_sub.label = GTK_LABEL(lbl_sub);
	fs_fonts_dir.label = GTK_LABEL(lbl_fd);
	
	gtk_register_dialog(dialog);
	
	gtk_table_attach_defaults(GTK_TABLE(tbl), LABEL("Font scale"), 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(tbl), LABEL("Line spacing"), 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(tbl), LABEL("Top margin"), 0, 1, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(tbl), LABEL("Bottom margin"), 0, 1, 3, 4);
	gtk_table_attach_defaults(GTK_TABLE(tbl), e_fs, 1, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(tbl), e_ls, 1, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(tbl), e_tm, 1, 2, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE(tbl), e_bm, 1, 2, 3, 4);
	
	gtk_box_pack_start(GTK_BOX(vbox), hb_sub, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), tbl, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hb_fd, TRUE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(hb_sub), LABEL("Subtitles file: "), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hb_sub), lbl_sub, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_sub), btn_sub, FALSE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(hb_fd), cb, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hb_fd), lbl_fd, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_fd), btn_fd, FALSE, FALSE, 0);
	
	gtk_label_set_text(GTK_LABEL(lbl_sub), (char*)_params->subfile);
	gtk_label_set_text(GTK_LABEL(lbl_fd), (char*)_params->fonts_dir);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(e_fs), _params->font_scale);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(e_ls), _params->line_spacing);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(e_tm), _params->top_margin);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(e_bm), _params->bottom_margin);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb), _params->extract_embedded_fonts);
	if(!_params->extract_embedded_fonts)
		gtk_widget_set_sensitive(btn_fd, FALSE);
	
	g_signal_connect(G_OBJECT(cb), "clicked", G_CALLBACK(cb_clicked), (void*)btn_fd);
	g_signal_connect(G_OBJECT(btn_sub), "clicked", G_CALLBACK(btn_sub_clicked), (void*)&fs_sub);
	g_signal_connect(G_OBJECT(btn_fd), "clicked", G_CALLBACK(btn_sub_clicked), (void*)&fs_fonts_dir);
	
	gtk_widget_show_all(vbox);
	while(gtk_dialog_run(GTK_DIALOG(dialog)) == 1 && *_params->subfile == 0)
		GUI_Error_HIG(_("You must select one file"), NULL);
	
	gtk_unregister_dialog(dialog);
	gtk_widget_hide(dialog);
	
	_params->extract_embedded_fonts = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb));
	// _params->fonts_dir and _params->subfile set by callbacks
	_params->top_margin = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e_tm));
	_params->bottom_margin = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e_bm));
	_params->font_scale = gtk_spin_button_get_value(GTK_SPIN_BUTTON(e_fs));
	_params->line_spacing = gtk_spin_button_get_value(GTK_SPIN_BUTTON(e_ls));

	printf("\nASS config\n\tExtract embedded fonts:%d\n\tFonts dir:%s\n\tSub file:%s\n\t",
		_params->extract_embedded_fonts, (char*)_params->fonts_dir, (char*)_params->subfile);
	printf("Top margin:%d\n\tBottom margin:%d\n\tFont scale:%f\n\tLine spacing:%f\n\t", 
		_params->top_margin, _params->bottom_margin, _params->font_scale, _params->line_spacing);
	gtk_widget_destroy(dialog);
}

#endif /* USE_FREETYPE */
