#ifndef STOPMOCAP_H
#define STOPMOCAP_H
#define WITH_QT

#include <vector>
#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QPainter>
#include <QTimer>
#include "ui_stopmocap.h"
#include "device.h"
#include "config.h"

#define STOPMOCAP_ORGANISATION	"Patrick F.-Productions"
#define STOPMOCAP_CONFIGKEY		"StopMoCap"


class MyQSlider : public QSlider
{
	public:
		MyQSlider( Qt::Orientation orientation, QWidget * parent = 0)
		: QSlider(orientation,parent)
		{

		}
		CameraControl cont;
		int lastValue;
};

class MyQCheckBox : public QCheckBox
{
	public:
		CameraControl cont;
		bool lastValue;
};

class MyQComboBox : public QComboBox
{
	public:
		CameraControl cont;
		int lastValue;
};


class StopMoCap : public QWidget
{
    Q_OBJECT

public:
    StopMoCap(QWidget *parent = 0);
    ~StopMoCap();

private:
    Ui::StopMoCapClass ui;
    Config conf;
    int lastFrameNum;
    ppl7::grafix::Image lastFrame;
    std::vector<VideoDevice> Devices;
    std::vector<VideoFormat> Formats;
    std::vector<ppl7::grafix::Size> FrameSizes;
    QVBoxLayout *controlLayout;
    bool inPlayback;
    int playbackFrame;

    void grabFrame();
    int highestSceneFrame();
    bool eventFilter(QObject *target, QEvent *event);
    bool consumeEvent(QObject *target, QEvent *event);
    QTimer *Timer;
    QTimer *PlaybackTimer;
    Device cam;


public slots:
    void on_deviceComboBox_currentIndexChanged(int index);
    void on_formatComboBox_currentIndexChanged(int index);
    void on_useDevice_clicked();
    void on_timer_fired();
    void on_playbackTimer_fired();
    void on_captureButton_clicked();
    void on_selectDir_clicked();
    void on_sceneName_textChanged ( const QString & text );
    void on_sceneName_editingFinished();
    void on_createScene_clicked();
    void on_frameSlider_sliderPressed();
    void on_frameSlider_sliderReleased();
    void on_frameSlider_valueChanged ( int value );

    void on_playButton_clicked();
    void on_stopButton_clicked();

};

#endif // STOPMOCAP_H
