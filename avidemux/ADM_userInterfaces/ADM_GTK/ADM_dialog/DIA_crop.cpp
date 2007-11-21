/***************************************************************************
                          DIA_crop.cpp  -  description
                             -------------------

			    GUI for cropping including autocrop
			    +Revisted the Gtk2 way
			     +Autocrop now in RGB space (more accurate)

    begin                : Fri May 3 2002
    copyright            : (C) 2002/2007 by mean
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

#include "default.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "../ADM_toolkit_gtk/toolkit_gtk.h"
#include "../ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_assert.h"
#include "DIA_flyDialog.h"
#include "DIA_flyCrop.h"

static GtkWidget	*create_dialog1 (void);

/* Link between UI and core */
static gboolean 	ui_draw( void );
static void 		ui_autocrop (void );
static void 		ui_reset( void );
static void 		ui_upload(void);
static void  		ui_read ( void);
static void  		ui_update ( void);
static void 		ui_frame_changed( void );

static GtkWidget *dialog=NULL;

/* Prevent loop when updating value */

static int lock=0;
static flyCrop *myCrop=NULL;
//
//	Video is in YV12 Colorspace
//
//
int DIA_getCropParams(char *name, CROP_PARAMS *param, AVDMGenericVideoStream *in)
{
	uint32_t width, height;

	// Allocate space for green-ised video
	width = in->getInfo()->width;
	height = in->getInfo()->height;

	uint8_t ret=0;

	dialog=create_dialog1();
	gtk_register_dialog(dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), name);
	gtk_widget_show(dialog);
	
#define CONNECT(x,y,z) 	gtk_signal_connect(GTK_OBJECT(WID(x)), #y, GTK_SIGNAL_FUNC(z), NULL);

	CONNECT(drawingarea1,expose_event,ui_draw);
	CONNECT(buttonAutocrop,clicked,ui_autocrop);
	CONNECT(buttonReset,clicked,ui_reset);
	CONNECT(scale,value_changed,ui_frame_changed);
	                  
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonCancel), GTK_RESPONSE_CANCEL);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonOk), GTK_RESPONSE_OK);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonApply), GTK_RESPONSE_APPLY);

#define CONNECT_SPIN(x) CONNECT(spinbutton##x, value_changed,ui_update)
          
	CONNECT_SPIN(Top);
	CONNECT_SPIN(Left);
	CONNECT_SPIN(Right);
	CONNECT_SPIN(Bottom);

	myCrop=new flyCrop(width, height,in,WID(drawingarea1),WID(scale));
	myCrop->left=param->left;
	myCrop->right=param->right;
	myCrop->top=param->top;
	myCrop->bottom=param->bottom;
	myCrop->upload();	
	myCrop->sliderChanged();	

	int response;

	while((response = gtk_dialog_run(GTK_DIALOG(dialog))) == GTK_RESPONSE_APPLY)
	{
	  ui_update();
	}

	if(response==GTK_RESPONSE_OK)
	{
		ui_read( );
		param->left=myCrop->left;
		param->right=myCrop->right;
		param->top=myCrop->top;
		param->bottom=myCrop->bottom;
		ret=1;
	}

	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	delete myCrop;
	return ret;
}

/*
      Link GTK dialog and core
*/
void ui_frame_changed( void )
{
  myCrop->sliderChanged();
}
void ui_update(void)
{
  if(lock) return;
  myCrop->download();
  myCrop->process();
  myCrop->display();
}
/*---------------------------------------------------------------------------
	Actually draw the working frame on screen
*/
gboolean ui_draw( void )
{
        myCrop->display();
}
void ui_read (void )
{
        myCrop->download();
}
void ui_upload(void)
{
    myCrop->upload();
}
void ui_autocrop( void )
{
  myCrop->autocrop();
 
}
void ui_reset( void )
{
        myCrop->left=0;
        myCrop->right=0;
        myCrop->bottom=0;
        myCrop->top=0;
        
	myCrop->upload();
        myCrop->process();
        myCrop->display();
}

/*---------------------------------------------------------------------------
	Read entried from dialog box
*/

#define SPIN_GET(x,y) {x= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(spinbutton##y))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton##y)),(gfloat)x) ;}


//____________________________________
uint8_t flyCrop::upload(void)
{
        lock++;
        SPIN_SET(left,Left);
        SPIN_SET(right,Right);
        SPIN_SET(top,Top);
        SPIN_SET(bottom,Bottom);
        lock--;
        return 1;
}
uint8_t flyCrop::download(void)
{
        int reject=0;
        
                        SPIN_GET(left,Left);
                        SPIN_GET(right,Right);
                        SPIN_GET(top,Top);
                        SPIN_GET(bottom,Bottom);
                        
                        printf("%d %d %d %d\n",left,right,top,bottom);
                        
                        left&=0xffffe;
                        right&=0xffffe;
                        top&=0xffffe;
                        bottom&=0xffffe;
                        
                        if((top+bottom)>_h)
                                {
                                        top=bottom=0;
                                        reject=1;
                                }
                        if((left+right)>_w)
                                {
                                        left=right=0;
                                        reject=1;
                                }
                        if(reject)
                                upload();
}
// End common part
//--------------------------------------------
GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbuttonbox1;
  GtkWidget *buttonCancel;
  GtkWidget *buttonApply;
  GtkWidget *buttonOk;
  GtkWidget *hbuttonbox2;
  GtkWidget *buttonAutocrop;
  GtkWidget *buttonReset;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkObject *spinbuttonLeft_adj;
  GtkWidget *spinbuttonLeft;
  GtkObject *spinbuttonTop_adj;
  GtkWidget *spinbuttonTop;
  GtkObject *spinbuttonBottom_adj;
  GtkWidget *spinbuttonBottom;
  GtkObject *spinbuttonRight_adj;
  GtkWidget *spinbuttonRight;
  GtkWidget *scale;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), QT_TR_NOOP("Crop Settings"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);

  buttonCancel = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (buttonCancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonCancel);
  GTK_WIDGET_SET_FLAGS (buttonCancel, GTK_CAN_DEFAULT);

  buttonApply = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (buttonApply);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonApply);
  GTK_WIDGET_SET_FLAGS (buttonApply, GTK_CAN_DEFAULT);

  buttonOk = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (buttonOk);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonOk);
  GTK_WIDGET_SET_FLAGS (buttonOk, GTK_CAN_DEFAULT);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox2, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_START);

  buttonAutocrop = gtk_button_new_with_mnemonic (QT_TR_NOOP("AutoCrop"));
  gtk_widget_show (buttonAutocrop);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), buttonAutocrop);
  GTK_WIDGET_SET_FLAGS (buttonAutocrop, GTK_CAN_DEFAULT);

  buttonReset = gtk_button_new ();
  gtk_widget_show (buttonReset);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), buttonReset);
  GTK_WIDGET_SET_FLAGS (buttonReset, GTK_CAN_DEFAULT);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (buttonReset), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-undo", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic (QT_TR_NOOP("Reset"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  table1 = gtk_table_new (2, 4, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, FALSE, TRUE, 0);

  label2 = gtk_label_new (QT_TR_NOOP("Crop Left :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (QT_TR_NOOP("Crop Top:"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (QT_TR_NOOP("Crop Right:"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new (QT_TR_NOOP("Crop Bottom:"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  spinbuttonLeft_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonLeft = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonLeft_adj), 1, 0);
  gtk_widget_show (spinbuttonLeft);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonLeft, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonLeft), TRUE);

  spinbuttonTop_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonTop = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTop_adj), 1, 0);
  gtk_widget_show (spinbuttonTop);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTop, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTop), TRUE);

  spinbuttonBottom_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonBottom = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBottom_adj), 1, 0);
  gtk_widget_show (spinbuttonBottom);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonBottom, 3, 4, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBottom), TRUE);

  spinbuttonRight_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonRight = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonRight_adj), 1, 0);
  gtk_widget_show (spinbuttonRight);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonRight, 3, 4, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonRight), TRUE);

  scale = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 110, 1, 10, 10)));
  gtk_widget_show (scale);
  gtk_box_pack_start (GTK_BOX (vbox1), scale, FALSE, TRUE, 0);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (vbox1), drawingarea1, FALSE, TRUE, 0);
  gtk_widget_set_size_request (drawingarea1, 100, 100);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbuttonbox1, "hbuttonbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonCancel, "buttonCancel");
  GLADE_HOOKUP_OBJECT (dialog1, buttonApply, "buttonApply");
  GLADE_HOOKUP_OBJECT (dialog1, buttonOk, "buttonOk");
  GLADE_HOOKUP_OBJECT (dialog1, hbuttonbox2, "hbuttonbox2");
  GLADE_HOOKUP_OBJECT (dialog1, buttonAutocrop, "buttonAutocrop");
  GLADE_HOOKUP_OBJECT (dialog1, buttonReset, "buttonReset");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonLeft, "spinbuttonLeft");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTop, "spinbuttonTop");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBottom, "spinbuttonBottom");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonRight, "spinbuttonRight");
  GLADE_HOOKUP_OBJECT (dialog1, scale, "scale");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");

  return dialog1;
}

