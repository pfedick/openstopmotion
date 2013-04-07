#include "ledslider.h"

LedSlider::LedSlider(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	ui.lineEdit->setText(QString::number(ui.slider->value()));
}


LedSlider::~LedSlider()
{

}


void LedSlider::setColor(const ppl7::grafix::Color &c)
{
	ppl7::String s;
	s.setf("background-color: rgb(%d,%d,%d);",c.red(),c.green(),c.blue());
	ui.colorFrame->setStyleSheet(s);

}

void LedSlider::setName(const QString &name)
{
	ui.label->setText(name);
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
	emit valueChanged(myId, ui.slider->value());
}

void LedSlider::on_lineEdit_editingFinished()
{
	ui.slider->setValue(ui.lineEdit->text().toInt());
}

void LedSlider::on_setKey_clicked()
{
	emit keyFrameSet(myId,ui.slider->value());
}

void LedSlider::on_delKey_clicked()
{
	emit keyFrameDelete(myId);
}


