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


