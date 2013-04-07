/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick
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

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <ppl7.h>

class Arduino
{
	private:
		int fd;
		int listener;
		int resettime;

		void writeBytes(const char *buffer, int size);

	public:
		Arduino();
		~Arduino();
		bool open(const ppl7::String device, int baud);
		void setCounter(int value);
		void setLight(int num, int value);
		void allLightsOn();
		void allLightsOff();
		void startDemo();
};



#endif /* ARDUINO_H_ */
