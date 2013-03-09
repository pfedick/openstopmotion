/*
 * arduino.h
 *
 *  Created on: 09.03.2013
 *      Author: patrick
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
