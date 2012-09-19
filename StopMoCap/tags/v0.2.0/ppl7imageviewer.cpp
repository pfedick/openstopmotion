/*
 * ppl7imageviewer.cpp
 *
 *  Created on: 09.06.2012
 *      Author: patrick
 */

#include "ppl7imageviewer.h"
#include <QPainter>
#include <QImage>

PPL7ImageViewer::PPL7ImageViewer(QWidget *parent)
	:QGLWidget(parent)
{
	img=NULL;
	smode=None;
}

void PPL7ImageViewer::setDrawable(ppl7::grafix::Drawable *draw)
{
	img=draw;
}

void PPL7ImageViewer::setScaling(ScalingMode mode)
{
	smode=mode;
}

PPL7ImageViewer::ScalingMode PPL7ImageViewer::scalingMode() const
{
	return smode;
}


void PPL7ImageViewer::paintEvent(QPaintEvent *)
{
	if (!img) return;
	QPainter painter(this);
	QPoint p(0,0);
	switch (smode) {
		case None:
		{
			QImage qi((uchar*)img->adr(),img->width(),img->height(), img->pitch(), QImage::Format_RGB32);
			painter.drawImage(p,qi);
			break;
		}
		case Fast:
		{
			int nw,nh;
			float ratio=(float)img->width()/(float)img->height();
			if (height()*ratio>width()) {
				nw=width();
				nh=(int)((float)nw/ratio);
			} else {
				nh=height();
				nw=(int)((float)nh*ratio);
			}
			p.setX((width()-nw)/2);
			p.setY((height()-nh)/2);


			QImage qi((uchar*)img->adr(),img->width(),img->height(), img->pitch(), QImage::Format_RGB32);
			QImage sc=qi.scaled(width(),height(),Qt::KeepAspectRatio,Qt::FastTransformation);
			painter.drawImage(p,sc);
			break;
		}
		case Smooth:
		{
			int nw,nh;
			float ratio=(float)img->width()/(float)img->height();
			if (height()*ratio>width()) {
				nw=width();
				nh=(int)((float)nw/ratio);
			} else {
				nh=height();
				nw=(int)((float)nh*ratio);
			}
			p.setX((width()-nw)/2);
			p.setY((height()-nh)/2);


			QImage qi((uchar*)img->adr(),img->width(),img->height(), img->pitch(), QImage::Format_RGB32);
			QImage sc=qi.scaled(width(),height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
			painter.drawImage(p,sc);
			break;
		}

	}


}

