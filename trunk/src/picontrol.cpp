/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author: pafe1234@gmail.com $
 * $Revision: 464 $
 * $Date: 2013-04-14 18:40:34 +0200 (So, 14 Apr 2013) $
 * $Id: arduino.cpp 464 2013-04-14 16:40:34Z pafe1234@gmail.com $
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

#include "picontrol.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>  /* POSIX terminal control definitions */
#include <errno.h>


PiControl::PiControl()
{
}

PiControl::~PiControl()
{
	if (Socket.isConnected()) Socket.disconnect();
}


void PiControl::connect(const ppl7::String hostname, int port)
{
	if (Socket.isConnected()) Socket.disconnect();
	Socket.connect(hostname,port);
}

void PiControl::disconnect()
{
	if (Socket.isConnected()) Socket.disconnect();
}

bool PiControl::isConnected() const
{
	return Socket.isConnected();
}

void PiControl::speak(const ppl7::String &cmd)
{
	Socket.write(cmd);
	ppl7::String answer;
	Socket.read(answer,1024);
	//answer.printnl();
}

void PiControl::setPWM(int num, int value)
{
	ppl7::String Tmp;
	Tmp.setf("pwm;%d;%d", num, value);
	speak(Tmp);
}
