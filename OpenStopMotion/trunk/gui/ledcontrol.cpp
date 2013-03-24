#include "ledcontrol.h"
#include <QPainter>
#include <QImage>


LedControl::LedControl(QWidget *parent)
    : QWidget(parent)
{
	myColorScheme=0;
	ppl7::String Tmp;
	ui.setupUi(this);
	for (int i=0;i<12;i++) {
		slider[i]=new LedSlider;
		slider[i]->setId(i);
		Tmp.setf("LED %d",i+1);
		slider[i]->setName(Tmp);


		connect(slider[i], SIGNAL(valueChanged(int,int)), this, SLOT(on_slider_valueChanged_fired(int,int)));
		connect(slider[i], SIGNAL(keyFrameSet(int,int)), this, SLOT(on_keyFrameSet_fired(int,int)));
		connect(slider[i], SIGNAL(keyFrameDelete(int)), this, SLOT(on_keyFrameDelete_fired(int)));


		ui.sliderLayout->addWidget(slider[i]);
	}
	ui.frameview->installEventFilter(this);

	lc[0].set(255,0,0);
	lc[1].set(0,255,0);
	lc[2].set(0,0,255);
	lc[3].set(255,255,0);
	lc[4].set(255,0,255);
	lc[5].set(0,255,255);
	lc[6].set(127,0,0);
	lc[7].set(0,127,0);
	lc[8].set(0,0,127);
	lc[9].set(127,127,0);
	lc[10].set(127,0,127);
	lc[11].set(0,127,127);

}


LedControl::~LedControl()
{

}

void LedControl::resizeEvent ( QResizeEvent * event )
{
	//updateFrameView();
	QWidget::resizeEvent(event);
}

void LedControl::showEvent (  QShowEvent * event )
{
	QWidget::showEvent(event);
}

bool LedControl::eventFilter(QObject *obj, QEvent *event)
{
	if (obj==ui.frameview && event->type() == QEvent::Paint) {
		updateFrameView();
		return true;
	} else {
		// standard event processing
		return QObject::eventFilter(obj, event);
	}
}

void LedControl::setConfig (Config &conf)
{
	this->conf=&conf;
}

void LedControl::setArduino (Arduino &arduino)
{
	this->arduino=&arduino;
}

void LedControl::setColorScheme(int scheme)
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



void LedControl::on_connectButton_clicked()
{
	if (arduino->open(ui.serialDevice->text(),ui.serialBaud->text().toInt())) {
		arduino->allLightsOn();
		ppl7::MSleep(500);
		arduino->allLightsOff();
		arduino->setCounter(0);
		for (int i=0;i<12;i++) {
			if (slider[i]->state()) arduino->setLight(i,slider[i]->value());
		}
	}
}

void LedControl::on_slider_valueChanged_fired(int id, int value)
{
	//printf ("Slider: %d, Value: %d\n",id,value);
	arduino->setLight(id,value);
}

void LedControl::on_keyFrameSet_fired(int id, int value)
{
	keyframes[id].add(ui.frameSlider->value(),value);
	ui.frameview->update();
}

void LedControl::on_keyFrameDelete_fired(int id)
{
	keyframes[id].erase(ui.frameSlider->value());
	ui.frameview->update();
}

void LedControl::updateFrameView()
{
	int w=ui.frameview->width();
	int h=ui.frameview->height();

	if (w!=img.width() || h!=img.height()) {
		//printf ("Erstelle zeichenflaeche: %i x %i\n",w,h);
		img.create(w,h);
	}
	if (img.isEmpty()) return;
	if (myColorScheme==0) {
		img.cls(ppl7::grafix::Color(255,255,255));
	} else {
		img.cls(ppl7::grafix::Color(0,0,0));
	}

	int frame=ui.frameSlider->value();
	int startframe=0;
	int endframe=startframe+w/5;
	int scale=h-14;
	int bottom=h-1;

	for (int led=0;led<12;led++) {
		int lastvalue=0;
		int x=0;
		int y=bottom-led;
		for (int f=0;f<endframe;f++) {
			if (f>=startframe) {
				int value=lastvalue;
				if (keyframes[led].exists(f)) value=keyframes[led][f];
				img.line(x,y,x+5,bottom-led-(value*scale/255),lc[led]);
				x+=5;
				y=bottom-led-(value*scale/255);
			}
			if (keyframes[led].exists(f)) lastvalue=keyframes[led][f];
		}

	}

	QPainter painter(ui.frameview);
	QPoint p(0,0);
	QImage qi((uchar*)img.adr(),img.width(),img.height(), img.pitch(), QImage::Format_RGB32);
	painter.drawImage(p,qi);

}
