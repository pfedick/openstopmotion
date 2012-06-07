#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QtGui/QWidget>
#include <QSlider>
#include "ui_myslider.h"

class MySlider : public QWidget
{
    Q_OBJECT

public:
    MySlider(QWidget *parent = 0);
    MySlider(Qt::Orientation orientation, QWidget *parent = 0);
    ~MySlider();

    CameraControl cont;
    int lastValue;

private:
    Ui::MySliderClass ui;
};

#endif // MYSLIDER_H
