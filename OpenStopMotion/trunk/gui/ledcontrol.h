#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#define WITH_QT

#include <QtGui/QWidget>
#include <QSlider>
#include <QSizePolicy>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "arduino.h"
#include "config.h"
#include "ledslider.h"
#include "ui_ledcontrol.h"

class LedControl : public QWidget
{
    Q_OBJECT

public:
    LedControl(QWidget *parent = 0);
    ~LedControl();

    void setConfig (Config &conf);
    void setArduino (Arduino &conf);
    void setColorScheme(int scheme);


private:
    Ui::LedControlClass ui;
    Config *conf;
    Arduino *arduino;
    LedSlider *slider[12];

public slots:

	void on_connectButton_clicked();
	void on_slider_valueChangedfired(int id, int value);

};

#endif // LEDCONTROL_H
