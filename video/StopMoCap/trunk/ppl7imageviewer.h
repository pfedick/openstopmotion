/*
 * ppl7imageviewer.h
 *
 *  Created on: 09.06.2012
 *      Author: patrick
 */

#ifndef PPL7IMAGEVIEWER_H_
#define PPL7IMAGEVIEWER_H_

#include <QWidget>
#include <ppl7.h>
#include <ppl7-grafix.h>

class PPL7ImageViewer : public QWidget
{
		Q_OBJECT
	public:
		enum ScalingMode {
			None,
			Fast,
			Smooth
		};

		PPL7ImageViewer(QWidget *parent = 0);
		void setDrawable(ppl7::grafix::Drawable *draw);
		void setScaling(ScalingMode mode);

	private:
		ppl7::grafix::Drawable *img;
		ppl7::grafix::Image myImg;
		ScalingMode smode;


	protected:
		void paintEvent(QPaintEvent *event);
};


#endif /* PPL7IMAGEVIEWER_H_ */
