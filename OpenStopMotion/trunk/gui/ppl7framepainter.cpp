
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

void PPL7FramePainter::setBackground(const QColor &c)
{
	QPainter painter(this);
	QRect r=painter.window();
	painter.setBackground(c);
	painter.eraseRect(r);
}
