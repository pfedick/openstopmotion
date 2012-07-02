#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QtGui/QWidget>
#include <QSlider>
#include <QSizePolicy>
#include "device.h"
#include "ui_myslider.h"

class MySlider : public QWidget
{
    Q_OBJECT

public:
    MySlider(QWidget *parent = 0);
    MySlider(Qt::Orientation orientation, QWidget *parent = 0);
    ~MySlider();

    void setMinimum(int value);
    void setMaximum(int value);
    void setSingleStep(int value);
    void setPageStep(int value);
    void setSliderPosition(int value);

    int value() const;

    CameraControl cont;
    int lastValue;

private:
    Ui::MySliderClass ui;
    bool noRecursion;

public slots:
	void on_slider_valueChanged ( int value );
	void on_spinBox_valueChanged ( int value );
};

#endif // MYSLIDER_H
