#ifndef Q_chromashift_h
#define Q_chromashift_h

#include "ui_chromashift.h"
#include "ADM_image.h"
#include "ADM_videoFilter.h"
#include "ADM_videoFilter/ADM_vidChromaShift_param.h"
#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"
#include "DIA_flyChromaShift.h"

class Ui_chromaShiftWindow : public QDialog
{
	Q_OBJECT

protected: 
	int lock;

public:
	flyChromaShift *myCrop;
	ADM_QCanvas *canvas;
	Ui_chromaShiftWindow(CHROMASHIFT_PARAM *param,AVDMGenericVideoStream *in);
	~Ui_chromaShiftWindow();
	Ui_chromashiftDialog ui;

public slots:
	void gather(CHROMASHIFT_PARAM *param);

private slots:
	void sliderUpdate(int foo);
	void valueChanged(int foo);
};
#endif	// Q_chromashift_h
