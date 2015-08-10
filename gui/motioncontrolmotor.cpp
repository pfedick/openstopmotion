/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author: pafe1234@gmail.com $
 * $Revision: 430 $
 * $Date: 2013-04-07 20:57:01 +0200 (So, 07 Apr 2013) $
 * $Id: ledslider.cpp 430 2013-04-07 18:57:01Z pafe1234@gmail.com $
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

#include "motioncontrolmotor.h"

MotionControlMotor::MotionControlMotor(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
}


MotionControlMotor::~MotionControlMotor()
{

}

