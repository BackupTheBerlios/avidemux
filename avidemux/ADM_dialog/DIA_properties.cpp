/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */



#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "config.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

#include "callbacks.h"
#include "interface.h"
#include "ADM_gui2/support.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

extern const char *getStrFromAudioCodec( uint32_t codec);
static GtkWidget	*create_dialog (void);

void DIA_properties( void )
{

 char text[80];
 uint16_t hh, mm, ss, ms;
 GtkWidget *dialog;

    if (playing)
	return;

    text[0] = 0;
    if (!avifileinfo)
	return;

	dialog = create_dialog();
	gtk_transient(dialog);
#define FILL_ENTRY(x) gtk_label_set_text((GtkLabel *) lookup_widget(dialog,#x),text);
	sprintf(text, "%lu x %lu  ", avifileinfo->width,		avifileinfo->height);

	FILL_ENTRY(label_size);

	sprintf(text, "%2.3f fps", (float) avifileinfo->fps1000 / 1000.F);
	FILL_ENTRY(label_fps);
		
	sprintf(text, "%ld frames", avifileinfo->nb_frames);
	FILL_ENTRY(label_number);
		
      	sprintf(text, "%s ",      fourCC::tostring(avifileinfo->fcc));
	FILL_ENTRY(label_videofourcc);

	if (avifileinfo->nb_frames)
	  {
	      	frame2time(avifileinfo->nb_frames, avifileinfo->fps1000,
			 &hh, &mm, &ss, &ms);
	      	sprintf(text, "%02d:%02d:%02d.%03d", hh, mm, ss, ms);
	   	FILL_ENTRY(label_duration);	

	  }
	// Now audio
	WAVHeader *wavinfo=NULL;
	if (currentaudiostream) wavinfo=currentaudiostream->getInfo();
	  if(wavinfo)
	  {
	      
	      switch (wavinfo->channels)
		{
		case 1:
		    sprintf(text, "MONO");
		    break;
		case 2:
		    sprintf(text, "STEREO");
		    break;
		default:
		    sprintf(text, "????");
		    break;
		}
	     	FILL_ENTRY(label1_audiomode);
	     
	      	sprintf(text, "%lu Hz", wavinfo->frequency);
	     	FILL_ENTRY(label_fq);
		sprintf(text, "%lu Bps / %lu kbps", wavinfo->byterate,      wavinfo->byterate * 8 / 1000);
		FILL_ENTRY(label_bitrate);
		sprintf(text, "%s ", getStrFromAudioCodec(wavinfo->encoding));
	     	FILL_ENTRY(label1_audiofourcc);
		// Duration in seconds too
		if(currentaudiostream)
		{
			ms2time((currentaudiostream->getLength()*1000)/wavinfo->byterate,
				 &hh, &mm, &ss, &ms);
	      		sprintf(text, "%02d:%02d:%02d.%03d (%lu MBytes)", hh, mm, ss, ms
				,currentaudiostream->getLength()>>20);
		}
		FILL_ENTRY(label_audioduration);
	} else
	  {
	      sprintf(text, "NONE");
	      	FILL_ENTRY(label_fq);
	     	FILL_ENTRY(label1_audiomode);
	        FILL_ENTRY(label_bitrate);
	  	FILL_ENTRY(label1_audiofourcc);
	  }

	gtk_dialog_run(GTK_DIALOG(dialog));	
	gtk_widget_destroy(dialog);
		
}  
  
GtkWidget*
create_dialog (void)
{
  GtkWidget *dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *table1;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label_size;
  GtkWidget *label_fps;
  GtkWidget *label_number;
  GtkWidget *label_videofourcc;
  GtkWidget *label_duration;
  GtkWidget *label5;
  GtkWidget *label1;
  GtkWidget *table2;
  GtkWidget *label14;
  GtkWidget *label15;
  GtkWidget *label16;
  GtkWidget *label17;
  GtkWidget *label1_audiofourcc;
  GtkWidget *label1_audiomode;
  GtkWidget *label_bitrate;
  GtkWidget *label_fq;
  GtkWidget *label_audioduration;
  GtkWidget *label13;
  GtkWidget *label2;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;

  dialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog), _("Video informations"));
  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

  dialog_vbox1 = GTK_DIALOG (dialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  table1 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (notebook1), table1);

  label3 = gtk_label_new (_("Video Size"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Frame Rate"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label6 = gtk_label_new (_("Duration"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new (_("Codec FourCC"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  label_size = gtk_label_new ("");
  gtk_widget_show (label_size);
  gtk_table_attach (GTK_TABLE (table1), label_size, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label_size, 120, -1);
  gtk_label_set_justify (GTK_LABEL (label_size), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_size), 0, 0.5);

  label_fps = gtk_label_new ("");
  gtk_widget_show (label_fps);
  gtk_table_attach (GTK_TABLE (table1), label_fps, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_fps), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_fps), 0, 0.5);

  label_number = gtk_label_new ("");
  gtk_widget_show (label_number);
  gtk_table_attach (GTK_TABLE (table1), label_number, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_number), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_number), 0, 0.5);

  label_videofourcc = gtk_label_new ("");
  gtk_widget_show (label_videofourcc);
  gtk_table_attach (GTK_TABLE (table1), label_videofourcc, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_videofourcc), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_videofourcc), 0, 0.5);

  label_duration = gtk_label_new ("");
  gtk_widget_show (label_duration);
  gtk_table_attach (GTK_TABLE (table1), label_duration, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_duration), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_duration), 0, 0.5);

  label5 = gtk_label_new (_("Number of frames"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label5, 130, -1);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label1 = gtk_label_new (_("Video"));
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  table2 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (notebook1), table2);

  label14 = gtk_label_new (_("Mode"));
  gtk_widget_show (label14);
  gtk_table_attach (GTK_TABLE (table2), label14, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  label15 = gtk_label_new (_("BitRate"));
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table2), label15, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

  label16 = gtk_label_new (_("Frequency"));
  gtk_widget_show (label16);
  gtk_table_attach (GTK_TABLE (table2), label16, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label16), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label16), 0, 0.5);

  label17 = gtk_label_new (_("Duration"));
  gtk_widget_show (label17);
  gtk_table_attach (GTK_TABLE (table2), label17, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label17), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label17), 0, 0.5);

  label1_audiofourcc = gtk_label_new ("");
  gtk_widget_show (label1_audiofourcc);
  gtk_table_attach (GTK_TABLE (table2), label1_audiofourcc, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label1_audiofourcc, 120, -1);
  gtk_label_set_justify (GTK_LABEL (label1_audiofourcc), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1_audiofourcc), 0, 0.5);

  label1_audiomode = gtk_label_new ("");
  gtk_widget_show (label1_audiomode);
  gtk_table_attach (GTK_TABLE (table2), label1_audiomode, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1_audiomode), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1_audiomode), 0, 0.5);

  label_bitrate = gtk_label_new ("");
  gtk_widget_show (label_bitrate);
  gtk_table_attach (GTK_TABLE (table2), label_bitrate, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_bitrate), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_bitrate), 0, 0.5);

  label_fq = gtk_label_new ("");
  gtk_widget_show (label_fq);
  gtk_table_attach (GTK_TABLE (table2), label_fq, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_fq), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_fq), 0, 0.5);

  label_audioduration = gtk_label_new ("");
  gtk_widget_show (label_audioduration);
  gtk_table_attach (GTK_TABLE (table2), label_audioduration, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_audioduration), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_audioduration), 0, 0.5);

  label13 = gtk_label_new (_("Codec"));
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table2), label13, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label13, 130, -1);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

  label2 = gtk_label_new (_("Audio"));
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GTK_DIALOG (dialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog, dialog, "dialog");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog, notebook1, "notebook1");
  GLADE_HOOKUP_OBJECT (dialog, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog, label_size, "label_size");
  GLADE_HOOKUP_OBJECT (dialog, label_fps, "label_fps");
  GLADE_HOOKUP_OBJECT (dialog, label_number, "label_number");
  GLADE_HOOKUP_OBJECT (dialog, label_videofourcc, "label_videofourcc");
  GLADE_HOOKUP_OBJECT (dialog, label_duration, "label_duration");
  GLADE_HOOKUP_OBJECT (dialog, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog, label14, "label14");
  GLADE_HOOKUP_OBJECT (dialog, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog, label16, "label16");
  GLADE_HOOKUP_OBJECT (dialog, label17, "label17");
  GLADE_HOOKUP_OBJECT (dialog, label1_audiofourcc, "label1_audiofourcc");
  GLADE_HOOKUP_OBJECT (dialog, label1_audiomode, "label1_audiomode");
  GLADE_HOOKUP_OBJECT (dialog, label_bitrate, "label_bitrate");
  GLADE_HOOKUP_OBJECT (dialog, label_fq, "label_fq");
  GLADE_HOOKUP_OBJECT (dialog, label_audioduration, "label_audioduration");
  GLADE_HOOKUP_OBJECT (dialog, label13, "label13");
  GLADE_HOOKUP_OBJECT (dialog, label2, "label2");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog, closebutton1, "closebutton1");

  return dialog;
}

