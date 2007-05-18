#ifndef ADM_Q_SLIDER_H
#define ADM_Q_SLIDER_H
class ADM_QSlider : public QWidget
{
        Q_OBJECT
signals:
void valueChanged(int value);
public slots:
protected:
        QSlider *_slider;
        uint32_t _max,_A,_B;
        void paintEvent(QPaintEvent *ev);
        
public:
        ADM_QSlider(QWidget *father); 
        ~ADM_QSlider(); 
        
        void setA(uint32_t a);
        void setB(uint32_t a);
        void setNbFrames(uint32_t a);
        int  value() const;
         Qt::Orientation orientation() const;

    void setMinimum(int);
    int minimum() const;

    void setMaximum(int);
    int maximum() const;

public Q_SLOTS:
    void setValue(int);
    void setOrientation(Qt::Orientation);

};
#endif

