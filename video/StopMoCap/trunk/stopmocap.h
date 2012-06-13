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
#include "myslider.h"
#include "ppl7imageviewer.h"
#include "savethread.h"

#define STOPMOCAP_ORGANISATION	"Patrick F.-Productions"
#define STOPMOCAP_CONFIGKEY		"StopMoCap"
#define STOPMOCAP_APPNAME		"StopMotion Capture"
#define STOPMOCAP_COPYRIGHT		"(c) 2012 by Patrick Fedick"
#define STOPMOCAP_VERSION		"0.2"

void DisplayException(const ppl7::Exception &e, QWidget *object=NULL, const ppl7::String &msg=ppl7::String());
void DisplayException(const std::exception &e, QWidget *object=NULL, const ppl7::String &msg=ppl7::String());



//#define USERENDERTHREAD

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


#ifdef USERENDERTHREAD

class RenderThread : private ppl7::Thread
{
	private:
		ppl7::Mutex mutex;
		ppl7::Mutex signal;
		QImage pix;
		ppl7::grafix::Image grab;
		ppl7::grafix::Image lastFrame;
		bool frameReady;
		bool newFrame;
		float blendFactor;
		int w,h;

	public:
		RenderThread();
		~RenderThread();
		void start();
		void stop();
		virtual void threadMain();
		void grabNext(Device &cam);
		bool ready() const;
		QPixmap getFrame() const;
		void setBlendFactor(float f);
		void setLastFrame(const ppl7::grafix::Image &img);
		void setImageSize(int width, int height);
};
#endif


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
    SaveThread savethread;
#ifdef USERENDERTHREAD
    RenderThread rthread;
#endif
    QVBoxLayout *controlLayout;
    bool inPlayback;
    bool inPreviewMode;
    int playbackFrame;
    QTimer *Timer;
    QTimer *PlaybackTimer;
    ppl7::grafix::Image grabImg;

    Device cam;
    ppluint64 fpsTimer;
    int fpsCounter;

    void grabFrame();
    int highestSceneFrame();
    bool eventFilter(QObject *target, QEvent *event);
    bool consumeEvent(QObject *target, QEvent *event);
    void capture(ppl7::grafix::Image &img);

protected:
    void resizeEvent ( QResizeEvent * event );

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
    void on_undoButton_clicked();

    void on_previewButton_toggled ( bool checked );
    void on_zoom11_clicked ( bool checked );
    void on_zoomFast_clicked ( bool checked );
    void on_zoomSmooth_clicked ( bool checked );

    void on_deleteScene_clicked();
    void on_selectScene_clicked();

    void on_imageFormat_currentIndexChanged(int index);
    void on_jpegQualitySlider_valueChanged ( int value );
    void on_jpegQualitySpinBox_valueChanged ( int value );

};

#endif // STOPMOCAP_H
