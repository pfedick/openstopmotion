#include "myslider.h"

MySlider::MySlider(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	noRecursion=false;
}

MySlider::MySlider(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	ui.slider->setOrientation(orientation);
	ui.spinBox->setValue(ui.slider->value());
	ui.spinBox->setMaximum(ui.slider->maximum());
	ui.spinBox->setMinimum(ui.slider->minimum());
	noRecursion=false;
}

MySlider::~MySlider()
{

}


void MySlider::setMinimum(int value)
{
	ui.slider->setMinimum(value);
	ui.spinBox->setMinimum(value);
}

void MySlider::setMaximum(int value)
{
	ui.slider->setMaximum(value);
	ui.spinBox->setMaximum(value);

}

void MySlider::setSingleStep(int value)
{
	ui.slider->setSingleStep(value);
}

void MySlider::setPageStep(int value)
{
	ui.slider->setPageStep(value);
}


void MySlider::setSliderPosition(int value)
{
	ui.slider->setSliderPosition(value);
	ui.spinBox->setValue(value);
}

int MySlider::value() const
{
	return ui.slider->value();
}

void MySlider::on_slider_valueChanged ( int value )
{
	if (noRecursion) return;
	printf ("Slider Value changed: %i\n",value);
	noRecursion=true;
	ui.spinBox->setValue(value);
	noRecursion=false;

}

void MySlider::on_spinBox_valueChanged ( int value )
{
	if (noRecursion) return;
	printf ("spinBox Value changed: %i\n",value);
	noRecursion=true;
	ui.slider->setValue(value);
	noRecursion=false;
}
