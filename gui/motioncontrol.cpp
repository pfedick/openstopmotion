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
#include <ppl7-inet.h>


MotionControl::Device::Device()
{
	this->Type=DeviceTyoe_None;
}

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


MotionControl::DevicePWMLight::DevicePWMLight(const ppl7::String &Name, int id, int min, int max, int value)
	: MotionControl::Device(MotionControl::DeviceType_PWMLight, Name)
{
	this->myId=id;
	this->min=min;
	this->max=max;
	this->currentValue=value;
}

int MotionControl::DevicePWMLight::minimum() const
{
	return min;
}

int MotionControl::DevicePWMLight::maximum() const
{
	return max;
}

int MotionControl::DevicePWMLight::value() const
{
	return currentValue;
}

int MotionControl::DevicePWMLight::id() const
{
	return myId;
}


void MotionControl::DevicePWMLight::setValue(int value)
{
	currentValue=value;
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
	currentDevice=NULL;
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

	addDevice(new DeviceStepMotor("CameraMovement"));
	addDevice(new DevicePWMLight("LED 1",0,0,4095));
	addDevice(new DevicePWMLight("LED 2",1,0,4095));
	addDevice(new DevicePWMLight("LED 3",2,0,4095));
	addDevice(new DevicePWMLight("LED 4",3,0,4095));
	addDevice(new DevicePWMLight("LED 5",4,0,4095));
	addDevice(new DevicePWMLight("LED 6",5,0,4095));
	addDevice(new DevicePWMLight("LED 7",6,0,4095));
	addDevice(new DevicePWMLight("LED 8",7,0,4095));
	addDevice(new DevicePWMLight("LED 9",8,0,4095));
	addDevice(new DevicePWMLight("LED 10",9,0,4095));
	addDevice(new DevicePWMLight("LED 11",10,0,4095));
	addDevice(new DevicePWMLight("LED 12",11,0,4095));
	addDevice(new DevicePWMLight("LED 13",12,0,4095));
	addDevice(new DevicePWMLight("LED 14",13,0,4095));
	addDevice(new DevicePWMLight("LED 15",14,0,4095));
	addDevice(new DevicePWMLight("LED 16",15,0,4095));

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
	ppl7::String Tmp;
	this->conf=&conf;
	ui.picontrolHostnameLineEdit->setText(conf.MotionControlHostname);
	Tmp.setf("%d",conf.MotionControlPort);
	ui.picontrolPortLineEdit->setText(Tmp);
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

void MotionControl::addDevice(MotionControl::Device *device)
{
	std::map<ppl7::String,Device*>::const_iterator it;
	it=Devices.find(device->name());
	if (it != Devices.end()) {
		return;
	}
	Devices.insert(std::pair<ppl7::String,Device*>(device->name(),device));
	ui.devicesListWidget->addItem(device->name());
}

void MotionControl::removeCameraControls()
{
	std::map<ppl7::String,Device*>::const_iterator it;
	for (it=Devices.begin(); it!=Devices.end(); ++it) {
		if (it->second->type()==DeviceType_CameraControlInteger) {
			QString n=it->second->name();
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


void MotionControl::on_devicesListWidget_currentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous )
{
	ppl7::String Name=current->text();
	try {
		MotionControl::Device *device=Devices[Name];
		//Name.printnl();
		switch (device->type()) {
			case MotionControl::DeviceType_PWMLight:
				currentDevice=device;
				ui.stackedWidget->setCurrentIndex(0);
				ui.currentDeviceName->setText(tr("Light"));
				ui.currentDeviceTypeName->setText(device->name());
				ui.controlRangeHorizontalSlider->setValue(((DevicePWMLight*)device)->value());
				ui.controlRangeHorizontalSlider->setMinimum(((DevicePWMLight*)device)->minimum());
				ui.controlRangeHorizontalSlider->setMaximum(((DevicePWMLight*)device)->maximum());
				ui.controlRangeHorizontalSlider->setValue(((DevicePWMLight*)device)->value());
				ui.controlRangeSpinBox->setValue(((DevicePWMLight*)device)->value());
				ui.controlRangeSpinBox->setMinimum(((DevicePWMLight*)device)->minimum());
				ui.controlRangeSpinBox->setMaximum(((DevicePWMLight*)device)->maximum());
				ui.controlRangeSpinBox->setValue(((DevicePWMLight*)device)->value());
				break;
			case MotionControl::DeviceType_StepMotor:
				currentDevice=device;
				ui.stackedWidget->setCurrentIndex(1);
				ui.currentDeviceName->setText(tr("StepMotor"));
				ui.currentDeviceTypeName->setText(device->name());

			default:
				break;

		}

	} catch (...) {
	}
}

void selectDevicePWMLight(const ppl7::String &Name)
{

}

void MotionControl::on_controlRangeHorizontalSlider_valueChanged(int value)
{
	ui.controlRangeSpinBox->setValue(value);
}

void MotionControl::on_controlRangeSpinBox_valueChanged(int value)
{
	ui.controlRangeHorizontalSlider->setValue(value);
	if (!currentDevice) return;
	if (!currentDevice->type()==MotionControl::DeviceType_PWMLight) return;
	if (!pi.isConnected()) {
		try {
			pi.connect(ui.picontrolHostnameLineEdit->text(),
					ui.picontrolPortLineEdit->text().toInt());
		} catch (const ppl7::Exception &e) {
			e.print();
			return;
		}
	}
	try {
		pi.setPWM(((DevicePWMLight*)currentDevice)->id(),value);
	} catch (const ppl7::Exception &e) {
		e.print();
	}
	((DevicePWMLight*)currentDevice)->setValue(value);
}
