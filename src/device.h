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
		class MenuItem
		{
			public:
				MenuItem(int id, const ppl7::String &Name) {
					this->id=id;
					this->Name=Name;
				}
				int id;
				ppl7::String Name;
		};

		int id;
		ControlType type;
		ppl7::String Name;
		int min;
		int max;
		int step;
		int defaultValue;
		int flags;
		std::list <MenuItem> MenuItems;
};

struct buffer {
        void   *start;
        size_t  length;
};

class Device
{
	private:
#ifdef WIN32
#else
		enum io_method {
		        IO_METHOD_READ,
		        IO_METHOD_MMAP,
		        IO_METHOD_USERPTR,
		};

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
#endif

		bool captureRunning;
	public:
		double time_readFrame;
		double time_decompress;
		double time_total;


		Device();
		~Device();
		static void enumerateDevices(std::list<VideoDevice> &list);
		static void enumerateImageFormats(std::list<VideoFormat> &list, const VideoDevice &device);
		static void enumerateFrameSizes(std::list<ppl7::grafix::Size> &list, const VideoDevice &device, const VideoFormat &fmt);

		void open(const VideoDevice &dev);
		void startCapture(const VideoFormat &fmt, int width, int height);
		void startCapture(const VideoFormat &fmt, const ppl7::grafix::Size &size);
		//void stopCapture();

		void enumerateControls(std::list<CameraControl> &list);
		void setControlValue(const CameraControl &ctl, int value);
		int getControlValue(const CameraControl &ctl);
		int readFrame(ppl7::grafix::Image &img);
		bool isOpen() const;

		void close();

		PPL7EXCEPTION(InvalidDevice, Exception);
		PPL7EXCEPTION(InvalidFormat, Exception);
		PPL7EXCEPTION(DeviceNotOpen, Exception);
		PPL7EXCEPTION(SelectError, Exception);
		PPL7EXCEPTION(Timeout, Exception);
		PPL7EXCEPTION(ReadError, Exception);
		PPL7EXCEPTION(StreamingUnsupported, Exception);
		PPL7EXCEPTION(StreamOffFailed, Exception);
		PPL7EXCEPTION(StreamOnFailed, Exception);
		PPL7EXCEPTION(StreamBufferError, Exception);
		PPL7EXCEPTION(DeviceDoesNotSupportCapture, Exception);
		PPL7EXCEPTION(MMapUnsupported, Exception);
		PPL7EXCEPTION(UserpUnsupported, Exception);
		PPL7EXCEPTION(InsufficientBufferMemory, Exception);
		PPL7EXCEPTION(QueryBufFailed, Exception);
		PPL7EXCEPTION(MMapFailed, Exception);
		PPL7EXCEPTION(MUnmapFailed, Exception);
		PPL7EXCEPTION(BufferError, Exception);
		PPL7EXCEPTION(SetControlValueFailed, Exception);
		PPL7EXCEPTION(GetControlValueFailed, Exception);

};


#endif /* DEVICE_H_ */
