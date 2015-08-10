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

#include "osm.h"
#include "motioncontrol.h"
#include "stopmocap.h"
#include <QPainter>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>


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

