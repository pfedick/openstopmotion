/*
 * capture.h
 *
 *  Created on: 01.01.2012
 *      Author: patrick
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_
#define WITH_QT
#include <ppl7.h>
#include <windows.h>
#include <vfw.h>

class CaptureDevice
{
	public:
		ppl7::String	Name;
		ppl7::String	Version;
		int				Id;
};

class Capture
{
	private:
	public:
		Capture();
		~Capture();

		void enumerate(ppl7::List<CaptureDevice> &list);
};



#endif /* CAPTURE_H_ */
