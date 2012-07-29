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
#include "scenemanager.h"
#include "bluebox.h"

#define STOPMOCAP_ORGANISATION	"Patrick F.-Productions"
#define STOPMOCAP_CONFIGKEY		"OpenStopMotion"
#define STOPMOCAP_APPNAME		"OpenStopMotion"
#define STOPMOCAP_COPYRIGHT		"(c) 2012 by Patrick Fedick"
#define STOPMOCAP_VERSION		"0.5"


//#define USE_SCENEMANAGER

void DisplayException(const ppl7::Exception &e, QWidget *object=NULL, const ppl7::String &msg=ppl7::String());
void DisplayException(const std::exception &e, QWidget *object=NULL, const ppl7::String &msg=ppl7::String());



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
    SaveThread savethread;
    BlueBox bluebox;
#ifdef USE_SCENEMANAGER
    SceneManager scm;
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
    void UpdateColorKeyBG(ppl7::grafix::Color c);
    void UpdateColorKeyFG(ppl7::grafix::Color c);

protected:
    void resizeEvent ( QResizeEvent * event );
    void showEvent (  QShowEvent * event );
    void closeEvent (  QCloseEvent * event );

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

    void on_imageFormatComp_currentIndexChanged(int index);
    void on_jpegQualitySliderComp_valueChanged ( int value );
    void on_jpegQualitySpinBoxComp_valueChanged ( int value );


    void on_viewer_mouseClicked(int x, int y, ppl7::grafix::Color c);
    void on_tolNearSlider_valueChanged ( int value );
    void on_tolFarSlider_valueChanged ( int value );
    void on_spillSlider_valueChanged ( int value );
    void on_chromaBackgroundSelect_clicked();
    void on_chromaKeyingEnabled_toggled(bool checked);


    void on_foregroundEnabled_toggled(bool checked);
    void on_tolNearSliderFG_valueChanged ( int value );
    void on_tolFarSliderFG_valueChanged ( int value );
    void on_spillSliderFG_valueChanged ( int value );
    void on_chromaForegroundSelect_clicked();
    void on_bgColorSelect_clicked();

    void on_replaceChromaWithColor_toggled(bool checked);
    void on_replaceChromaWithImage_toggled(bool checked);
    void on_replaceChromaWithTransparent_toggled(bool checked);


};

#endif // STOPMOCAP_H
