

#ifndef PPL7FRAMEPAINTER_H_
#define PPL7FRAMEPAINTER_H_

#include <QWidget>
#include <QtOpenGL/QGLWidget>
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
		void mousePressEvent(QMouseEvent *event);



	signals:
	     void mouseClicked(int x, int y, ppl7::grafix::Color c);
};


#endif /* PPL7FRAMEPAINTER_H_ */
