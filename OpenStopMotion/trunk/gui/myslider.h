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

#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QtGui/QWidget>
#include <QSlider>
#include <QSizePolicy>
#include "device.h"
#include "ui_myslider.h"

class MySlider : public QWidget
{
    Q_OBJECT

public:
    MySlider(QWidget *parent = 0);
    MySlider(Qt::Orientation orientation, QWidget *parent = 0);
    ~MySlider();

    void setMinimum(int value);
    void setMaximum(int value);
    void setSingleStep(int value);
    void setPageStep(int value);
    void setSliderPosition(int value);

    int value() const;

    CameraControl cont;
    int lastValue;

private:
    Ui::MySliderClass ui;
    bool noRecursion;

public slots:
	void on_slider_valueChanged ( int value );
	void on_spinBox_valueChanged ( int value );
};

#endif // MYSLIDER_H
