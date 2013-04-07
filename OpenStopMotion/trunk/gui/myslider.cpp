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
	ui.spinBox->setSingleStep(value);
}

void MySlider::setPageStep(int value)
{
	ui.slider->setPageStep(value);
	//ui.spinBox->setPageStep(value);
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
	//printf ("Slider Value changed: %i\n",value);
	noRecursion=true;
	ui.spinBox->setValue(value);
	noRecursion=false;

}

void MySlider::on_spinBox_valueChanged ( int value )
{
	if (noRecursion) return;
	//printf ("spinBox Value changed: %i\n",value);
	noRecursion=true;
	ui.slider->setValue(value);
	noRecursion=false;
}
