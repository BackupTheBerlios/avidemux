#ifndef ADM_QCANVAS_H
#define ADM_QCANVAS_H
class  ADM_QCanvas : public QWidget
{
     Q_OBJECT
     signals:
        
        
   public slots:
   public :
         uint8_t *dataBuffer;
  protected:
         uint32_t _w,_h;
  public:
        ADM_QCanvas(QWidget *z,uint32_t w,uint32_t h)  ;
        ~ADM_QCanvas() ;
        void paintEvent(QPaintEvent *ev);
};
#endif

