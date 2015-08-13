/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author: pafe1234@gmail.com $
 * $Revision: 513 $
 * $Date: 2015-07-02 18:00:30 +0200 (Do, 02 Jul 2015) $
 * $Id: ledcontrol.h 513 2015-07-02 16:00:30Z pafe1234@gmail.com $
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

#ifndef MOTIONCONTROL_H
#define MOTIONCONTROL_H

#define WITH_QT

#include <QWidget>
#include <QSlider>
#include <QTimer>
#include <QSizePolicy>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "config.h"
#include "device.h"
#include "ui_motioncontrol.h"
#include <map>

class StopMoCap;

class MotionControl : public QWidget
{
    Q_OBJECT

public:
    enum DeviceType {
    	DeviceType_PWMLight,
    	DeviceType_StepMotor,
    	DeviceType_CameraControlInteger,

    };
    class Device
    {
    	private:
    		DeviceType Type;
    		ppl7::String Name;
    	public:
    		Device(DeviceType Type, const ppl7::String &Name);
    		const ppl7::String &name() const;
    		DeviceType type() const;

    };

    class DevicePWMLight : public Device
    {
    	private:
    		int min, max;
    	public:
    		DevicePWMLight(const ppl7::String &Name, int min, int max);
    };

    class DeviceStepMotor : public Device
    {
    	private:
    	public:
    		DeviceStepMotor(const ppl7::String &Name);

    };

    class DeviceCameraControlInteger : public Device
    {
    	private:
    		const CameraControl &control;
    	public:
    		DeviceCameraControlInteger(const CameraControl &control);
    };


    MotionControl(QWidget *parent = 0);
    ~MotionControl();

    void setMainCapture(StopMoCap *cap);
    void setConfig (Config &conf);
    void setColorScheme(int scheme);

    void addDevice(const MotionControl::Device &device);
    void removeDevice(const ppl7::String &Name);
    void removeCameraControls();

//    void load(const ppl7::String &filename);
//    void save(const ppl7::String &filename);

protected:
    void resizeEvent ( QResizeEvent * event );
    void showEvent (  QShowEvent * event );
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MotionControlClass ui;
    StopMoCap *cap;
    Config *conf;
    std::map<ppl7::String,Device> Devices;


    ppl7::grafix::Image img;
    int myColorScheme;
    ppl7::String Filename;

    /*
    void updateFrameView();
    void recalcFrames(int id);
    */
    bool unsaved;
    /*
    int  findNextKeyFrame(int led, int start);
    int  findPreviousKeyFrame(int led, int start);
    void remindSave();
    */

public slots:
	/*
	void on_connectButton_clicked();
	void on_slider_valueChanged_fired(int id, int value);
	void on_keyFrameSet_fired(int id, int value);
	void on_keyFrameDelete_fired(int id);
	void on_maxFrame_textChanged(const QString & text);
	void on_offsetFrame_textChanged(const QString & text);

	void on_frameSlider_valueChanged(int value);

	void on_clearButton_clicked();
	void on_loadButton_clicked();
	void on_saveButton_clicked();
	void on_saveAsButton_clicked();

	void on_frameNextButton_clicked();
	void on_frameBackButton_clicked();
	void on_keyNextButton_clicked();
	void on_keyBackButton_clicked();

	void on_playButton_clicked();
	void on_stopButton_clicked();
	void on_arduinoButton_clicked();

	void on_playbackTimer_fired();
	*/

};

#endif // MOTIONCONTROL_H
