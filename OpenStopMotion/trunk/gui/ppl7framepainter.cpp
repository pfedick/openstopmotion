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

#include "ppl7framepainter.h"
#include <QPainter>
#include <QImage>
#include <QMouseEvent>

PPL7FramePainter::PPL7FramePainter(QWidget *parent)
	:QGLWidget(parent)
{
	img=NULL;
	smode=None;
}

void PPL7FramePainter::setDrawable(ppl7::grafix::Drawable *draw)
{
	img=draw;
}

void PPL7FramePainter::setScaling(ScalingMode mode)
{
	smode=mode;
	QPainter painter(this);
	QRect r=painter.window();
	painter.eraseRect(r);
}

PPL7FramePainter::ScalingMode PPL7FramePainter::scalingMode() const
{
	return smode;
}

void PPL7FramePainter::paintEvent(QPaintEvent *)
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
/*
void PPL7FramePainter::mousePressEvent(QMouseEvent *event)
{
	if (!img) return;
	ppl7::grafix::Color c;
	QPoint p;
	p=event->pos();
	int x,y;
	switch (smode) {
		case None:
			x=p.x();
			y=p.y();
			break;
		case Fast:
		case Smooth:
			QPoint p2(0,0);
			int nw,nh;
			float ratio=(float)img->width()/(float)img->height();
			if (height()*ratio>width()) {
				nw=width();
				nh=(int)((float)nw/ratio);
			} else {
				nh=height();
				nw=(int)((float)nh*ratio);
			}
			p2.setX((width()-nw)/2);
			p2.setY((height()-nh)/2);
			//printf ("nw=%i, nh=%i\n",nw,nh);
			x=(p.x()-p2.x())*img->width()/nw;
			y=(p.y()-p2.y())*img->height()/nh;

			break;
	}

	c=img->getPixel(x,y);

	emit mouseClicked(x,y,c);

	//printf ("Event: %i/%i => %i/%i, Color: %i, %i, %i\n",p.x(),p.y(),x,y,c.red(),c.green(),c.blue());
	QWidget::mousePressEvent(event);
}
*/

void PPL7FramePainter::setBackground(const QColor &c)
{
	QPainter painter(this);
	QRect r=painter.window();
	painter.setBackground(c);
	painter.eraseRect(r);
}
