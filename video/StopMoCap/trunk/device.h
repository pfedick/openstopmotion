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

class CameraControl
{
	public:
		enum ControlType {
			Integer,
			Boolean,
			Menu,
			Unknown
		};
		int id;
		ControlType type;
		ppl7::String Name;
		int min;
		int max;
		int step;
		int defaultValue;
		int flags;
};

struct buffer {
        void   *start;
        size_t  length;
};


class Device
{
	private:
		static void enumerateDevice(const ppl7::String &DeviceName, int index, VideoDevice &d);
		void waitForNextFrame();
		void initCapture(size_t buffer_size);
		void stopCapturing();
		void startCapturing();
		void freeBuffers();

		struct buffer          *buffers;
		VideoDevice dev;
		VideoFormat fmt;
		ppl7::grafix::Size size;
		int myff;
		bool captureRunning;

	public:
		Device();
		~Device();
		static void enumerateDevices(std::list<VideoDevice> &list);
		static void enumerateImageFormats(std::list<VideoFormat> &list, const VideoDevice &device);
		static void enumerateFrameSizes(std::list<ppl7::grafix::Size> &list, const VideoDevice &device, const VideoFormat &fmt);

		void open(const VideoDevice &dev);
		void startCapture(const VideoFormat &fmt, int width, int height);
		void startCapture(const VideoFormat &fmt, const ppl7::grafix::Size &size);
		void stopCapture();

		void enumerateControls(std::list<CameraControl> &list);
		void readFrame(ppl7::ByteArray &ba);

		void close();

		PPLNORMALEXCEPTION(InvalidDevice);
		PPLNORMALEXCEPTION(InvalidFormat);
		PPLNORMALEXCEPTION(DeviceNotOpen);
		PPLNORMALEXCEPTION(SelectError);
		PPLNORMALEXCEPTION(Timeout);
		PPLNORMALEXCEPTION(ReadError);
		PPLNORMALEXCEPTION(StreamingUnsupported);
		PPLNORMALEXCEPTION(StreamOffError);
		PPLNORMALEXCEPTION(StreamOnError);
		PPLNORMALEXCEPTION(StreamBufferError);

};


#endif /* DEVICE_H_ */
