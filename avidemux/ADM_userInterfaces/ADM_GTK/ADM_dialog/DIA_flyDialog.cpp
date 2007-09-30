/***************************************************************************
    copyright            : (C) 2006 by mean
    email                : fixounet@free.fr
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************///

#include "default.h"

#include <gtk/gtk.h>

#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "DIA_flyDialog.h"
#include "DIA_factory.h"
#include "ADM_assert.h"

extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);
extern float UI_calcZoomToFitScreen(GtkWindow* window, GtkWidget* drawingArea, uint32_t imageWidth, uint32_t imageHeight);
extern void UI_centreCanvasWindow(GtkWindow *window, GtkWidget *canvas, int newCanvasWidth, int newCanvasHeight);

void ADM_flyDialog::postInit(void)
{
	if (_slider)
	{
		GtkAdjustment *adj = (GtkAdjustment*)gtk_adjustment_new(0, 0, _in->getInfo()->nb_frames - 1, 0, 1, 0);

		gtk_range_set_adjustment(GTK_RANGE(_slider), adj);
		gtk_scale_set_digits(GTK_SCALE(_slider), 0);
	}

	GtkWindow *window = (GtkWindow*)gtk_widget_get_ancestor((GtkWidget*)_canvas, GTK_TYPE_WINDOW);
	UI_centreCanvasWindow(window, (GtkWidget*)_canvas, _zoomW, _zoomH);
	gtk_widget_set_usize((GtkWidget*)_canvas, _zoomW, _zoomH);
}

float ADM_flyDialog::calcZoomFactor(void)
{
	GtkWindow *window = (GtkWindow*)gtk_widget_get_ancestor((GtkWidget*)_canvas, GTK_TYPE_WINDOW);

	return UI_calcZoomToFitScreen(window, (GtkWidget*)_canvas, _w, _h);
}

uint8_t  ADM_flyDialog::display(void)
{
	ADM_assert(_canvas);
	ADM_assert(_rgbBufferOut);

	GUI_RGBDisplay(_rgbBufferOut, _zoomW, _zoomH, _canvas);

	return 1; 
}

uint32_t ADM_flyDialog::sliderGet(void)
{
	ADM_assert(_slider);
	GtkAdjustment *adj=gtk_range_get_adjustment (GTK_RANGE(_slider));
	return (uint32_t)GTK_ADJUSTMENT(adj)->value;
}

uint8_t ADM_flyDialog::sliderSet(uint32_t value)
{
	ADM_assert(_slider);

	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(_slider));

	adj->value = value;

	gtk_range_set_adjustment(GTK_RANGE(_slider), adj);

	return 1; 
}

uint8_t ADM_flyDialog::isRgbInverted(void)
{
	return 0; 
}

void ADM_flyDialog::setupMenus (const void * params, 
                                const MenuMapping * menu_mapping,
                                uint32_t menu_mapping_count)
{
    // Fill in menus

    ADM_assert (_cookie); // should be the dialog pointer
    GtkDialog * dialog = static_cast <GtkDialog *> (_cookie);

    for (int menu_index = 0; menu_index < menu_mapping_count; menu_index++)
    {
        const MenuMapping & mm = menu_mapping [menu_index];
        const uint32_t * theParam
            = (reinterpret_cast <const uint32_t *>
               (mm.paramOffset + reinterpret_cast <const char *> (params)));
        // printf ("setupMenus: %s: params %p + offset 0x%x = %p\n",
        //         mm.widgetName, params, mm.paramOffset, theParam);

        GtkComboBox * combo
            = GTK_COMBO_BOX (lookup_widget (GTK_WIDGET(dialog),
                                            mm.widgetName));
        int found = -1;
        for (int item_index = 0; item_index < mm.count; item_index++)
        {
            gtk_combo_box_append_text (combo, mm.menu [item_index].text);
            if (mm.menu [item_index].val == *theParam)
            {
                gtk_combo_box_set_active (combo, item_index);
                found = item_index;
            }
        }

        // printf ("setupMenus: %s: current is %d (%d)\n",
        //         mm.widgetName, found, *theParam);
    }
}

uint32_t ADM_flyDialog::getMenuValue (const MenuMapping * mm)
{
    ADM_assert (_cookie); // should be the dialog pointer
    GtkDialog * dialog = static_cast <GtkDialog *> (_cookie);
    GtkComboBox * combo
        = GTK_COMBO_BOX (lookup_widget (GTK_WIDGET(dialog), mm->widgetName));
    uint32_t currValue = gtk_combo_box_get_active (combo);
    ADM_assert (currValue < mm->count);

    return (mm->menu [currValue].val);
}

void ADM_flyDialog::getMenuValues (void * params,
                                   const MenuMapping * menu_mapping,
                                   uint32_t menu_mapping_count)
{
    for (int menu_index = 0; menu_index < menu_mapping_count; menu_index++)
    {
        const MenuMapping & mm = menu_mapping [menu_index];
        uint32_t * theParam = reinterpret_cast <uint32_t *>
                              (mm.paramOffset
                               + reinterpret_cast <char *> (params));
        *theParam = getMenuValue (&mm);
    }
}

const MenuMapping *
ADM_flyDialog::lookupMenu (const char * widgetName,
                           const MenuMapping * menu_mapping,
                           uint32_t menu_mapping_count)
{
    for (int menu_index = 0; menu_index < menu_mapping_count; menu_index++)
    {
        const MenuMapping * mm = menu_mapping + menu_index;
        if (strcmp (mm->widgetName, widgetName) == 0)
            return mm;
    }

    return 0;
}

//EOF
