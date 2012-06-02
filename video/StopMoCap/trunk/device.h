/*
 * device.h
 *
 *  Created on: 17.12.2011
 *      Author: patrickf
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <list>
#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>



class VideoFormat
{
	public:
		ppl7::String	Description;
		int				pixelformat;
		int				flags;
};

class VideoDevice
{
	public:
		ppl7::String DeviceName;
		int index;
		ppl7::String Name;
};

class Device
{
	private:
		static void enumerateDevice(const ppl7::String &DeviceName, int index, VideoDevice &d);

		VideoDevice dev;
		VideoFormat fmt;
		ppl7::grafix::Size size;
		int myff;

	public:
		Device();
		~Device();
		static void enumerateDevices(std::list<VideoDevice> &list);
		static void enumerateImageFormats(std::list<VideoFormat> &list, const VideoDevice &device);
		static void enumerateFrameSizes(std::list<ppl7::grafix::Size> &list, const VideoDevice &device, const VideoFormat &fmt);

		void open(const VideoDevice &dev);
		void setCaptureFormat(const VideoFormat &fmt, int width, int height);
		void setCaptureFormat(const VideoFormat &fmt, const ppl7::grafix::Size &size);


		void close();

		PPLNORMALEXCEPTION(InvalidDevice);
		PPLNORMALEXCEPTION(InvalidFormat);
		PPLNORMALEXCEPTION(DeviceNotOpen);
};


#endif /* DEVICE_H_ */
