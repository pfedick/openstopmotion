#ifndef STOPMOCAP_H
#define STOPMOCAP_H

#include <vector>
#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include "ui_stopmocap.h"
#define WITH_QT
#include "device.h"


class MyQSlider : public QSlider
{
	public:
		MyQSlider( Qt::Orientation orientation, QWidget * parent = 0)
		: QSlider(orientation,parent)
		{

		}
		CameraControl cont;
};

class MyQCheckBox : public QCheckBox
{
	public:
		CameraControl cont;
};

class StopMoCap : public QWidget
{
    Q_OBJECT

public:
    StopMoCap(QWidget *parent = 0);
    ~StopMoCap();

private:
    Ui::StopMoCapClass ui;
    Device cam;
    std::vector<VideoDevice> Devices;
    std::vector<VideoFormat> Formats;
    std::vector<ppl7::grafix::Size> FrameSizes;
    QVBoxLayout *controlLayout;


public slots:
    void on_deviceComboBox_currentIndexChanged(int index);
    void on_formatComboBox_currentIndexChanged(int index);
    void on_useDevice_clicked();
};

#endif // STOPMOCAP_H
