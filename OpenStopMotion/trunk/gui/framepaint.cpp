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

#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>

#include "framepaint.h"
#include <QTreeWidgetItem>

FramePaint::FramePaint(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	conf=NULL;
	fBuffer=NULL;
	lastFrameNum=0;
	inPlayback=false;
	interpolateSequence=0;
	PlaybackTimer=new QTimer(this);
	connect(PlaybackTimer, SIGNAL(timeout()), this, SLOT(on_playbackTimer_fired()));
	ui.viewer->setDrawable(&displayFrame);
}

FramePaint::~FramePaint()
{

}

void FramePaint::setFrameBuffer(FrameBuffer &buffer)
{
	this->fBuffer=&buffer;
}

void FramePaint::setConfig (Config &conf)
{
	this->conf=&conf;
}

void FramePaint::setMaxFrameNumber(int num)
{
	lastFrameNum=num;
	if (lastFrameNum>0) {
		ui.frameSlider->setMaximum(lastFrameNum);
		updateCurrentFrame(1);
	}
}

void FramePaint::setFrameRate(int fps)
{
	ui.frameRate->setValue(fps);
}

void FramePaint::setDarkColorScheme(bool flag)
{
	ui.lightAndDarkButton->setChecked(flag);
}

void FramePaint::init()
{
	if (!conf) return;
	if (!fBuffer) return;
	ui.frameSlider->setMinimum(1);
	ui.frameSlider->setMaximum(1);
	ui.frameSlider->setSliderPosition(1);
	ui.frameRate->setValue(conf->frameRate);
	inPlayback=false;
	interpolateSequence=0;


}

void FramePaint::updateCurrentFrame(int frame)
{
	fBuffer->copyImage(frame,currentFrame);
	int overlay=ui.overlaySlider->value();
	//if (overlay>-10 && overlay<10) overlay=0;
	if (overlay>0 && frame<lastFrameNum) {
		float blendFactor=1.0f-(float)(overlay)/10.0f;
		fBuffer->copyImage(frame+1,displayFrame);
		displayFrame.bltBlend(currentFrame,blendFactor);

	} else if (overlay<0 && frame>1) {
		overlay*=-1;
		float blendFactor=1.0f-(float)(overlay)/10.0f;
		fBuffer->copyImage(frame-1,displayFrame);
		displayFrame.bltBlend(currentFrame,blendFactor);

	} else {
		displayFrame.copy(currentFrame);
	}
}


void FramePaint::on_frameSlider_sliderPressed()
{
	if (lastFrameNum<1) return;
	//playbackFrame=1;
	//inPlayback=true;
	on_frameSlider_valueChanged((int)ui.frameSlider->value());
}

void FramePaint::on_frameSlider_sliderReleased()
{
	//inPlayback=false;
}


void FramePaint::on_frameSlider_valueChanged ( int value )
{
	if (lastFrameNum<1) return;
	ppl7::String Tmp;
	Tmp.setf("%i",value);
	ui.frameNum->setText(Tmp);
	//if (!inPlayback) return;

	updateCurrentFrame(value);

	ui.viewer->update();
}

void FramePaint::on_frameFirstButton_clicked()
{
	ui.frameSlider->setValue(1);
}

void FramePaint::on_frameLastButton_clicked()
{
	ui.frameSlider->setValue(lastFrameNum);
}

void FramePaint::on_frameBackButton_clicked()
{
	int frame=ui.frameSlider->value();
	if (frame>1) ui.frameSlider->setValue(frame-1);
}

void FramePaint::on_frameForwardButton_clicked()
{
	int frame=ui.frameSlider->value();
	if (frame<lastFrameNum) ui.frameSlider->setValue(frame+1);
}


void FramePaint::on_playButton_clicked()
{
	interpolateSequence=0;
	inPlayback=true;
	//playbackFrame=1;
	PlaybackTimer->start(1000/ui.frameRate->value());
}

void FramePaint::on_stopButton_clicked()
{
	inPlayback=false;
	PlaybackTimer->stop();
}

void FramePaint::on_playbackTimer_fired()
{
	int frame=ui.frameSlider->value();
	if (frame<lastFrameNum) frame++;
	else frame=1;
	ui.frameSlider->setValue(frame);
}

void FramePaint::on_frameRate_valueChanged(int value)
{
	if (inPlayback) {
		PlaybackTimer->stop();
		PlaybackTimer->start(1000/value);
	}
}

void FramePaint::on_overlaySlider_valueChanged ( int value )
{
	updateCurrentFrame(ui.frameSlider->value());
	ui.viewer->update();
	ppl7::String v;
	v.setf("%d",value);
	ui.onionSkinValue->setText(v);
}


void FramePaint::on_lightAndDarkButton_toggled(bool checked)
{
	if (checked) {
		setStyleSheet("background-color: rgb(48, 48, 48);\ncolor: rgb(175, 175, 175);");
		ui.viewer->setBackground(QColor(0,0,0));

	} else {
		setStyleSheet("");
		ui.viewer->setBackground(QColor(233,232,230));
	}


}

