/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author: pafe1234@gmail.com $
 * $Revision: 513 $
 * $Date: 2015-07-02 18:00:30 +0200 (Do, 02 Jul 2015) $
 * $Id: ledslider.h 513 2015-07-02 16:00:30Z pafe1234@gmail.com $
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

#ifndef MOTIONCONTROLMOTOR_H
#define MOTIONCONTROLMOTOR_H
#define WITH_QT
#include <QWidget>
#include <QSlider>
#include <QSizePolicy>
#include "device.h"
#include "ui_ledslider.h"

class MotionControlMotor : public QWidget
{
    Q_OBJECT

public:
    MotionControlMotor(QWidget *parent = 0);
    ~MotionControlMotor();


private:
    Ui::LedSliderClass ui;
    /*
    int myId;

public slots:
	void on_slider_valueChanged ( int value );
	void on_lineEdit_editingFinished();
	void on_setKey_clicked();
	void on_delKey_clicked();


signals:
	void valueChanged(int id, int value);
	void keyFrameSet(int id, int value);
	void keyFrameDelete(int id);
	*/

};

#endif // MOTIONCONTROLMOTOR_H
