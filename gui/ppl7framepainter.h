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

#ifndef PPL7FRAMEPAINTER_H_
#define PPL7FRAMEPAINTER_H_

#include <QWidget>
#include <QGLWidget>
#include <ppl7.h>
#include <ppl7-grafix.h>

class PPL7FramePainter : public QGLWidget
{
		Q_OBJECT
	public:
		enum ScalingMode {
			None,
			Fast,
			Smooth
		};

		PPL7FramePainter(QWidget *parent = 0);
		void setDrawable(ppl7::grafix::Drawable *draw);
		void setScaling(ScalingMode mode);
		ScalingMode scalingMode() const;
		void setBackground(const QColor &c);

	private:
		ppl7::grafix::Drawable *img;
		ppl7::grafix::Image myImg;
		ScalingMode smode;

	protected:
		void paintEvent(QPaintEvent *event);
		//void mousePressEvent(QMouseEvent *event);



	signals:
	     //void mouseClicked(int x, int y, ppl7::grafix::Color c);

};


#endif /* PPL7FRAMEPAINTER_H_ */
