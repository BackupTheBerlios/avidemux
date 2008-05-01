#ifndef DIA_flyDialogQt4_h
#define DIA_flyDialogQt4_h

#include <QtGui/QWidget>

class ADM_QCanvas : public QWidget
{
protected:
	uint32_t _w,_h;
public:
	uint8_t *dataBuffer;

	ADM_QCanvas(QWidget *z, uint32_t w, uint32_t h);
	~ADM_QCanvas();
	void paintEvent(QPaintEvent *ev);
	void changeSize(uint32_t w, uint32_t h);
};

#endif	// DIA_flyDialogQt4_h
