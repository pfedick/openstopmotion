/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author: pafe1234@gmail.com $
 * $Revision: 477 $
 * $Date: 2014-02-20 11:10:11 +0100 (Do, 20 Feb 2014) $
 * $Id: MotionControl.cpp 477 2014-02-20 10:10:11Z pafe1234@gmail.com $
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

#define WITH_QT
#include "osm.h"
#include "motioncontrol.h"
#include "stopmocap.h"
#include <QPainter>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <map>


MotionControl::Device::Device(DeviceType Type, const ppl7::String &Name)
{
	this->Type=Type;
	this->Name=Name;
}

const ppl7::String & MotionControl::Device::name() const
{
	return Name;
}

MotionControl::DeviceType MotionControl::Device::type() const
{
	return Type;
}


MotionControl::DevicePWMLight::DevicePWMLight(const ppl7::String &Name, int min, int max)
	: MotionControl::Device(MotionControl::DeviceType_PWMLight, Name)
{
	this->min=min;
	this->max=max;
}

MotionControl::DeviceStepMotor::DeviceStepMotor(const ppl7::String &Name)
	: MotionControl::Device(MotionControl::DeviceType_StepMotor, Name)
{

}

MotionControl::DeviceCameraControlInteger::DeviceCameraControlInteger(const CameraControl &control)
	: MotionControl::Device(MotionControl::DeviceType_CameraControlInteger, ppl7::String("Cam_")+control.Name),
	  control(control)
{

}


MotionControl::MotionControl(QWidget *parent)
    : QWidget(parent)
{
	cap=NULL;
	unsaved=false;
	//PlaybackTimer=new QTimer(this);
	myColorScheme=0;
	ppl7::String Tmp;
	ui.setupUi(this);
/*
	for (int i=0;i<12;i++) {
		slider[i]=new LedSlider;
		slider[i]->setId(i);
		Tmp.setf("LED %d",i+1);
		slider[i]->setName(Tmp);
		slider[i]->setColor(lc[i]);


		connect(slider[i], SIGNAL(valueChanged(int,int)), this, SLOT(on_slider_valueChanged_fired(int,int)));
		connect(slider[i], SIGNAL(keyFrameSet(int,int)), this, SLOT(on_keyFrameSet_fired(int,int)));
		connect(slider[i], SIGNAL(keyFrameDelete(int)), this, SLOT(on_keyFrameDelete_fired(int)));


		ui.sliderLayout->addWidget(slider[i]);
	}
	*/
	ui.frameview->installEventFilter(this);
	//connect(PlaybackTimer, SIGNAL(timeout()), this, SLOT(on_playbackTimer_fired()));

	addDevice(DeviceStepMotor("CameraMovement"));
	addDevice(DevicePWMLight("LED 1",0,4095));
	addDevice(DevicePWMLight("LED 2",0,4095));
	addDevice(DevicePWMLight("LED 3",0,4095));
	addDevice(DevicePWMLight("LED 4",0,4095));
	addDevice(DevicePWMLight("LED 5",0,4095));
	addDevice(DevicePWMLight("LED 6",0,4095));
	addDevice(DevicePWMLight("LED 7",0,4095));
	addDevice(DevicePWMLight("LED 8",0,4095));
	addDevice(DevicePWMLight("LED 9",0,4095));
	addDevice(DevicePWMLight("LED 10",0,4095));
	addDevice(DevicePWMLight("LED 11",0,4095));
	addDevice(DevicePWMLight("LED 12",0,4095));
	addDevice(DevicePWMLight("LED 13",0,4095));
	addDevice(DevicePWMLight("LED 14",0,4095));
	addDevice(DevicePWMLight("LED 15",0,4095));
	addDevice(DevicePWMLight("LED 16",0,4095));

}


MotionControl::~MotionControl()
{
	//PlaybackTimer->stop();
	//delete PlaybackTimer;
	//if (unsaved) remindSave();
}

void MotionControl::resizeEvent ( QResizeEvent * event )
{
	//updateFrameView();
	QWidget::resizeEvent(event);
}

void MotionControl::showEvent (  QShowEvent * event )
{
	QWidget::showEvent(event);
}

bool MotionControl::eventFilter(QObject *obj, QEvent *event)
{
	if (obj==ui.frameview && event->type() == QEvent::Paint) {
		//updateFrameView();
		return true;
	} else {
		// standard event processing
		return QObject::eventFilter(obj, event);
	}
}

void MotionControl::setConfig (Config &conf)
{
	this->conf=&conf;
	if (conf.MotionControlFile.notEmpty()) {
		//load (conf.MotionControlFile);
		Filename=conf.MotionControlFile;
		unsaved=false;
	}
}

void MotionControl::setColorScheme(int scheme)
{
	if (scheme==1) {
		myColorScheme=1;
		setStyleSheet("background-color: rgb(48, 48, 48);\ncolor: rgb(175, 175, 175);");
		//ui.viewer->setBackground(QColor(0,0,0));
	} else {
		myColorScheme=0;
		setStyleSheet("");
		//ui.viewer->setBackground(QColor(233,232,230));
	}
}

void MotionControl::setMainCapture(StopMoCap *cap)
{
	this->cap=cap;
}

void MotionControl::addDevice(const MotionControl::Device &device)
{
	std::map<ppl7::String,Device>::const_iterator it;
	it=Devices.find(device.name());
	if (it != Devices.end()) {
		return;
	}
	Devices.insert(std::pair<ppl7::String,Device>(device.name(),device));
	ui.devicesListWidget->addItem(device.name());
}

void MotionControl::removeCameraControls()
{
	std::map<ppl7::String,Device>::const_iterator it;
	for (it=Devices.begin(); it!=Devices.end(); ++it) {
		if (it->second.type()==DeviceType_CameraControlInteger) {
			QString n=it->second.name();
			for (int row=0;row<ui.devicesListWidget->count();row++) {
				QListWidgetItem *item=ui.devicesListWidget->item(row);
				if (item->text()==n) {
					item=ui.devicesListWidget->takeItem(row);
					delete item;
					row=0;
				}
			}
			Devices.erase(it->first);
		}
	}
}
