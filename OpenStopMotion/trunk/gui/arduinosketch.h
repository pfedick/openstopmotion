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

#ifndef ARDUINOSKETCH_H
#define ARDUINOSKETCH_H

#define WITH_QT

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "ui_arduinosketch.h"
#include "osm.h"


class ArduinoSketch : public QDialog
{
    Q_OBJECT

private:
    Ui::ArduinoSketchClass ui;

public:
    ArduinoSketch(QWidget *parent = 0)
    : QDialog(parent)
    {
    	setWindowTitle("Arduino Sketch for LED Lightcontrol");
    	ui.setupUi(this);
    	QFile file(":/other/arduino_ledcontrol/arduino_ledcontrol.ino");
    	file.open(QIODevice::ReadOnly | QIODevice::Text);
   		QTextStream in(&file);
   		QString line = in.readAll();
   		ui.plainTextEdit->setPlainText(line);
    }

    ~ArduinoSketch()
    {

    }


public slots:

	void on_closeButton_clicked()
	{
		done(1);
	}

};

#endif // ARDUINOSKETCH_H
