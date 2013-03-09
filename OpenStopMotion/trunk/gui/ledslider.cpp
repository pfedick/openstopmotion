#include "ledslider.h"

LedSlider::LedSlider(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
}


LedSlider::~LedSlider()
{

}

void LedSlider::setName(const QString &name)
{
	ui.label->setText(name);
}

void LedSlider::setState(bool onOff)
{
	ui.onOff->setDown(onOff);
}

void LedSlider::setValue(int value)
{
	ui.slider->setSliderPosition(value);
	ui.lineEdit->setText(QString::number(value));
}

int LedSlider::value() const
{
	return ui.slider->value();
}

void LedSlider::setId(int id)
{
	myId=id;
}

int LedSlider::id() const
{
	return myId;
}


void LedSlider::on_slider_valueChanged ( int value )
{
	ui.lineEdit->setText(QString::number(value));
}

void LedSlider::on_lineEdit_editingFinished()
{

}

void LedSlider::on_setKey_clicked()
{

}

void LedSlider::on_delKey_clicked()
{

}


