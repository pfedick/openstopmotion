#ifndef LEDSLIDER_H
#define LEDSLIDER_H
#define WITH_QT
#include <QtGui/QWidget>
#include <QSlider>
#include <QSizePolicy>
#include "device.h"
#include "ui_ledslider.h"

class LedSlider : public QWidget
{
    Q_OBJECT

public:
    LedSlider(QWidget *parent = 0);
    ~LedSlider();

    void setName(const QString &name);
    void setId(int id);
    void setValue(int value);
    void setState(bool onOff);

    int value() const;
    int id() const;
    bool state() const;


private:
    Ui::LedSliderClass ui;
    int myId;

public slots:
	void on_slider_valueChanged ( int value );
	void on_lineEdit_editingFinished();
	void on_setKey_clicked();
	void on_delKey_clicked();
	void on_onOff_clicked();


signals:
	void valueChanged(int id, int value);
	void keyFrameSet(int id, int value);
	void keyFrameDelete(int id);

};

#endif // LEDSLIDER_H
