/***************************************************************************
                     Q_x264_zones.cpp  -  description
                     --------------------------------

                      GUI for configuring x264 zones

    begin                : Wed Apr 02 2008
    copyright            : (C) 2008 by gruntster
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
#include "ui_x264_zones.h"

class x264ZonesDialog : public QDialog
{
	Q_OBJECT

private:
	Ui_x264ZonesDialog ui;

public:
	x264ZonesDialog();

private slots:

};

x264ZonesDialog::x264ZonesDialog()
{
	ui.setupUi(this);
}
