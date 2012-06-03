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
		enum Capabilities {
			CAP_VIDEO_CAPTURE			= 0x00000001,
			CAP_READWRITE				= 0x00000002,
			CAP_STREAMING				= 0x00000004,
			CAP_VIDEO_OUTPUT_OVERLAY	= 0x00000008,
			CAP_ASYNCIO					= 0x00000010,
		};

		ppl7::String DeviceName;
		int index;
		ppl7::String Name;
		int caps;
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
		void initMMap();
		void initUserp(size_t buffer_size);

		void stopCapturing();
		void startCapturing();
		void initBuffers(int n, size_t buffer_size);
		void freeBuffers();
		void processImage(void *buffer, size_t size, ppl7::grafix::Image &img);

		struct buffer          *buffers;
		unsigned int n_buffers;
		VideoDevice dev;
		VideoFormat fmt;
		ppl7::grafix::Size size;
		int myff;
		int iomethod;
		bool captureRunning;

	public:
		enum io_method {
		        IO_METHOD_READ,
		        IO_METHOD_MMAP,
		        IO_METHOD_USERPTR,
		};

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
		int readFrame(ppl7::grafix::Image &img);

		void close();

		PPLNORMALEXCEPTION(InvalidDevice);
		PPLNORMALEXCEPTION(InvalidFormat);
		PPLNORMALEXCEPTION(DeviceNotOpen);
		PPLNORMALEXCEPTION(SelectError);
		PPLNORMALEXCEPTION(Timeout);
		PPLNORMALEXCEPTION(ReadError);
		PPLNORMALEXCEPTION(StreamingUnsupported);
		PPLNORMALEXCEPTION(StreamOffFailed);
		PPLNORMALEXCEPTION(StreamOnFailed);
		PPLNORMALEXCEPTION(StreamBufferError);
		PPLNORMALEXCEPTION(DeviceDoesNotSupportCapture);
		PPLNORMALEXCEPTION(MMapUnsupported);
		PPLNORMALEXCEPTION(UserpUnsupported);
		PPLNORMALEXCEPTION(InsufficientBufferMemory);
		PPLNORMALEXCEPTION(QueryBufFailed);
		PPLNORMALEXCEPTION(MMapFailed);
		PPLNORMALEXCEPTION(MUnmapFailed);
		PPLNORMALEXCEPTION(BufferError);

};


#endif /* DEVICE_H_ */
