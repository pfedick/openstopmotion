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
	printf ("RenderThread::start\n");
	frameReady=false;
	newFrame=false;
	while(!threadShouldStop()) {
		printf ("RenderThread: Waiting for new image\n");
		signal.wait(5000);
		printf ("RenderThread: Signal received\n");
		if (threadShouldStop()) return;
		mutex.lock();
		if (newFrame) {
			if (blendFactor>0.0f && lastFrame.isEmpty()==false) {
				grab.blend(lastFrame,blendFactor);
			}
			QImage qi((uchar*)grab.adr(),grab.width(),grab.height(), grab.pitch(), QImage::Format_RGB32);
			pix=qi.scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation);
			newFrame=false;
			frameReady=true;
		}
		mutex.unlock();
	}
	printf ("RenderThread::end\n");
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
	printf ("RenderThread::grabNext\n");
	mutex.lock();
	printf ("cam.readFrame\n");
	cam.readFrame(grab);
	newFrame=true;
	printf ("ok, signalisiere Renderthread\n");
	mutex.unlock();
	signal.signal();
}

bool RenderThread::ready() const
{
	return frameReady;
}

QPixmap RenderThread::getFrame() const
{
	printf ("RenderThread::getFrame\n");
	return QPixmap::fromImage(pix);
}

void RenderThread::setBlendFactor(float f)
{
	mutex.lock();
	blendFactor=f;
	mutex.unlock();
}

void RenderThread::setLastFrame(const ppl7::grafix::Image &img)
{
	mutex.lock();
	lastFrame=img;
	mutex.unlock();
}

void RenderThread::start()
{
	printf("RenderThread::start\n");
	threadStart();
}

void RenderThread::stop()
{
	threadStop();
}


#endif

