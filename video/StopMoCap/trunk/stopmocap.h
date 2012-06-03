#ifndef STOPMOCAP_H
#define STOPMOCAP_H
#define WITH_QT

#include <vector>
#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QPainter>
#include <QTimer>
#include "ui_stopmocap.h"
#include "device.h"

#define STOPMOCAP_ORGANISATION	"Patrick F.-Productions"
#define STOPMOCAP_CONFIGKEY		"StopMoCap"

class Config
{
	public:
		Config();
		~Config();
		void load();
		void save();


		ppl7::String	CaptureDir;
		ppl7::String	Scene;
		int mergeFrames;
		int skipFrames;
		int	onionValue;

};

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

    void grabFrame();
    QTimer *Timer;

    Config conf;

public slots:
    void on_deviceComboBox_currentIndexChanged(int index);
    void on_formatComboBox_currentIndexChanged(int index);
    void on_useDevice_clicked();
    void on_timer_fired();
    void on_captureButton_clicked();
    void on_selectDir_clicked();
};

#endif // STOPMOCAP_H
