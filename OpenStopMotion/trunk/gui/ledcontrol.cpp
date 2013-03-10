#include "ledcontrol.h"

LedControl::LedControl(QWidget *parent)
    : QWidget(parent)
{
	ppl7::String Tmp;
	ui.setupUi(this);
	for (int i=0;i<12;i++) {
		slider[i]=new LedSlider;
		slider[i]->setId(i);
		Tmp.setf("LED %d",i+1);
		slider[i]->setName(Tmp);


		connect(slider[i], SIGNAL(valueChanged(int,int)), this, SLOT(on_slider_valueChangedfired(int,int)));


		ui.sliderLayout->addWidget(slider[i]);
	}
}


LedControl::~LedControl()
{

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
		setStyleSheet("background-color: rgb(48, 48, 48);\ncolor: rgb(175, 175, 175);");
		//ui.viewer->setBackground(QColor(0,0,0));
	} else {
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

	}

}

void LedControl::on_slider_valueChangedfired(int id, int value)
{
	//printf ("Slider: %d, Value: %d\n",id,value);
	arduino->setLight(id,value);
}
