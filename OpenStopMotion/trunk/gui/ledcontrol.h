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

protected:
    void resizeEvent ( QResizeEvent * event );
    void showEvent (  QShowEvent * event );
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::LedControlClass ui;
    Config *conf;
    Arduino *arduino;
    LedSlider *slider[12];
    ppl7::grafix::Image img;
    int myColorScheme;
    ppl7::grafix::Color lc[12];

    ppl7::AVLTree<int, int> keyframes[12];

    void updateFrameView();

public slots:

	void on_connectButton_clicked();
	void on_slider_valueChanged_fired(int id, int value);
	void on_keyFrameSet_fired(int id, int value);
	void on_keyFrameDelete_fired(int id);

};

#endif // LEDCONTROL_H
