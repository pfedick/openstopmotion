/*
 * renderthread.cpp
 *
 *  Created on: 07.06.2012
 *      Author: patrick
 */


#define WITH_QT
#include <ppl7.h>
#include "stopmocap.h"
#include "device.h"
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QFileDialog>
#include <QEvent>

#ifdef USERENDERTHREAD

RenderThread::RenderThread()
{
	frameReady=false;
	newFrame=false;
	blendFactor=0.0f;
	w=0;
	h=0;
}

RenderThread::~RenderThread()
{
	signal.signal();
}

void RenderThread::threadMain()
{
	frameReady=false;
	newFrame=false;
	while(!threadShouldStop()) {
		signal.wait(1000);
		if (threadShouldStop()) return;
		mutex.lock();
		if (newFrame) {
			if (blendFactor>0.0f && lastFrame.isEmpty()==false) {
				grab.blend(lastFrame,blendFactor);
			}
			QImage qi((uchar*)grab.adr(),grab.width(),grab.height(), grab.pitch(), QImage::Format_RGB32);
			pix=QPixmap::fromImage(qi).scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation);
			newFrame=false;
			frameReady=true;
		}
		mutex.unlock();
	}
}

void RenderThread::setImageSize(int width, int height)
{
	mutex.lock();
	w=width;
	h=height;
	mutex.unlock();
}

void RenderThread::grabNext(Device &cam)
{
	mutex.lock();
	cam.readFrame(grab);
	newFrame=true;
	signal.signal();
	mutex.unlock();
}

bool RenderThread::ready() const
{
	return frameReady;
}

const QPixmap& RenderThread::getFrame() const
{
	return pix;
}

void RenderThread::setBlendFactor(float f)
{
	mutex.lock();
	blendFactor=f;
	mutex.unlock();
}

void StopMoCap::grabFrame()
{
	ppl7::grafix::Image img;
	cam.readFrame(img);
	//img.create(1280,720);
	float blendFactor=(float)ui.onionSkinning->value()/99.0f;
	if (blendFactor>0.0f) {
		img.blend(lastFrame,blendFactor);
	}
	//ppl7::grafix::ImageFilter_PNG png;
	//png.saveFile("test.png",img);
	QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
	QPixmap pm=QPixmap::fromImage(qi);
	ui.viewer->setPixmap(pm.scaled(ui.viewer->width(),ui.viewer->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
	return;
}

#endif

