/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick <patrick@pfp.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STOPMOCAP_H
#define STOPMOCAP_H
#define WITH_QT

#include <vector>
#include <QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QPainter>
#include <QTimer>
#include <QStatusBar>
#include <QThread>
#include "ui_stopmocap.h"
#include "device.h"
#include "config.h"
#include "myslider.h"
#include "ppl7imageviewer.h"
#include "savethread.h"
#include "FrameBuffer.h"
#include "bluebox.h"
#include "framepaint.h"
#include "arduino.h"
#include "ledcontrol.h"
#include "motioncontrol.h"


//#define USE_SCENEMANAGER

void DisplayException(const ppl7::Exception &e, QWidget *object=NULL, const ppl7::String &msg=ppl7::String());
void DisplayException(const std::exception &e, QWidget *object=NULL, const ppl7::String &msg=ppl7::String());

class StopMoCap;


class MotorThread : public QThread
{
	Q_OBJECT
		 signals:
		 	void motorStarted();
		 	void motorStopped();

	private:
		StopMoCap *gui;
		ppl7::String Uri;

	public:
		MotorThread(StopMoCap *gui);
		void getUri(const ppl7::String &Uri);
		void run();

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
    SaveThread savethread;
    BlueBox bluebox;
    FrameBuffer fBuffer;
    QVBoxLayout *controlLayout;
    bool inPlayback;
    bool inPreviewMode;
    int playbackFrame;
    QTimer *Timer;
    QTimer *PlaybackTimer;
    ppl7::grafix::Image grabImg;
    int interpolateSequence;
    Device cam;
    ppluint64 fpsTimer;
    int fpsCounter;
    FramePaint *fpaint;
    Arduino arduino;
    LedControl *ledcontrol;
    MotionControl *motioncontrol;
    MotorThread *motorThread;

    QStatusBar *statusBar;
    QLabel *statusbar_fps;
    QLabel *statusbar_time_grab;
    QLabel *statusbar_time_decode;
    QLabel *statusbar_time_total;

    void grabFrame();
    int highestSceneFrame();
    bool eventFilter(QObject *target, QEvent *event);
    bool consumeEvent(QObject *target, QEvent *event);
    void capture(ppl7::grafix::Image &img);
    void UpdateColorKeyBG(ppl7::grafix::Color c);
    void UpdateColorKeyFG(ppl7::grafix::Color c);
    void getSceneList(ppl7::Array &scenes);
    void createStatusBar();
    void updateStatusBar();

public:
    void stopCapture();
    void startCapture();
    bool isCaptureActive();

protected:
    void resizeEvent ( QResizeEvent * event );
    void showEvent (  QShowEvent * event );
    void closeEvent (  QCloseEvent * event );

public slots:
    void on_deviceComboBox_currentIndexChanged(int index);
    void on_formatComboBox_currentIndexChanged(int index);
    void on_useDevice_clicked();
    void on_arduinoButton_clicked();
    void on_motionControlButton_clicked();
    void on_timer_fired();
    void on_playbackTimer_fired();
    void on_captureButton_clicked();
    void on_captureBackgroundButton_clicked();
    void on_captureOverblendButton_clicked();
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
    void on_frameRate_valueChanged(int value);

    void on_previewButton_toggled ( bool checked );
    void on_zoom11_clicked ( bool checked );
    void on_zoomFast_clicked ( bool checked );
    void on_zoomSmooth_clicked ( bool checked );

    void on_sceneUpButton_clicked();
    void on_sceneDownButton_clicked();


    void on_deleteScene_clicked();
    void on_selectScene_clicked();

    void on_imageFormat_currentIndexChanged(int index);
    void on_jpegQualitySlider_valueChanged ( int value );
    void on_jpegQualitySpinBox_valueChanged ( int value );

    void on_imageFormatComp_currentIndexChanged(int index);
    void on_jpegQualitySliderComp_valueChanged ( int value );
    void on_jpegQualitySpinBoxComp_valueChanged ( int value );


    void on_viewer_mouseClicked(int x, int y, ppl7::grafix::Color c);
    void on_chromaKeyColorSelect_clicked();
    void on_tolNearSlider_valueChanged ( int value );
    void on_tolFarSlider_valueChanged ( int value );
    void on_spillSlider_valueChanged ( int value );
    void on_chromaBackgroundSelect_clicked();
    void on_chromaKeyingEnabled_toggled(bool checked);


    void on_foregroundEnabled_toggled(bool checked);
    void on_chromaKeyFGColorSelect_clicked();
    void on_tolNearSliderFG_valueChanged ( int value );
    void on_tolFarSliderFG_valueChanged ( int value );
    void on_spillSliderFG_valueChanged ( int value );
    void on_chromaForegroundSelect_clicked();
    void on_bgColorSelect_clicked();

    void on_replaceChromaWithColor_toggled(bool checked);
    void on_replaceChromaWithImage_toggled(bool checked);
    void on_replaceChromaWithTransparent_toggled(bool checked);

    void on_lightAndDarkButton_toggled(bool checked);

    void on_releaseButton_clicked();
    //void on_editScene_clicked();

    // Webcontrol
    void on_motorMoveLeft_clicked();
    void on_motorMoveRight_clicked();
    void on_motorTurnLeft_clicked();
    void on_motorTurnRight_clicked();

 	void motorStarted();
 	void motorStopped();


};

#endif // STOPMOCAP_H
