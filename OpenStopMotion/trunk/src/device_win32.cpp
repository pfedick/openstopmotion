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

#ifdef WIN32
#include "device.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <windows.h>
#include <Vfw.h>
//#include <dshow.h>


Device::Device()
{
	captureRunning=false;
}

Device::~Device()
{
	close();
}

void Device::enumerateDevices(std::list<VideoDevice> &list)
{
	ppl7::String DeviceName;
	list.clear();
	wchar_t szDeviceName[80];
	wchar_t szDeviceVersion[80];

	for (int wIndex = 0; wIndex < 64; wIndex++)
	{
	    if (capGetDriverDescription(
	            wIndex,
	            szDeviceName,
	            sizeof (szDeviceName),
	            szDeviceVersion,
	            sizeof (szDeviceVersion)
	        ))
	    {
	    	VideoDevice d;
	    	d.index=wIndex;
	    	d.DeviceName.setf("%i",wIndex);
	    	d.Name.set(szDeviceName);
	    	list.push_back(d);

	        // Append name to list of installed capture drivers
	        // and then let the user select a driver to use.
	    }
	}
	/*
	for (int d=0;d<64;d++) {
		DeviceName.setf("/dev/video%i",d);
		for (int i=0;i<64;i++) {

			try {
				VideoDevice d;
				enumerateDevice(DeviceName,i,d);
				list.push_back(d);
			} catch (const ppl7::FileObject::FileNotFoundException &e) {
			} catch (const Device::InvalidDevice &e) {

			} catch (...) {
				throw;
			}
		}
	}
	*/
}


void Device::enumerateImageFormats(std::list<VideoFormat> &list, const VideoDevice &device)
{
	/*
	int ff=::open((const char*)device.DeviceName, O_RDONLY);
	if (ff==-1) ppl7::File::throwErrno(errno,device.DeviceName);

	struct v4l2_fmtdesc fmt;
	memset (&fmt, 0, sizeof (fmt));
	fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

	while (0 == xioctl (ff, VIDIOC_ENUM_FMT, &fmt)) {
		//printf ("Standardname: %s\n", fmt.description);
		if (fmt.pixelformat==V4L2_PIX_FMT_JPEG || fmt.pixelformat==V4L2_PIX_FMT_MJPEG) {
			VideoFormat f;
			f.Description.set((const char*)fmt.description);
			f.pixelformat=fmt.pixelformat;
			f.flags=fmt.flags;
			list.push_back(f);
		}
		fmt.index++;
	}
	::close(ff);
	*/
}

void Device::enumerateFrameSizes(std::list<ppl7::grafix::Size> &list, const VideoDevice &device, const VideoFormat &fmt)
{
	/*
	int ff=::open((const char*)device.DeviceName, O_RDONLY);
	if (ff==-1) ppl7::File::throwErrno(errno,device.DeviceName);
	struct v4l2_frmsizeenum s;
	memset (&s, 0, sizeof (s));
	s.pixel_format=fmt.pixelformat;


	while (0 == xioctl (ff, VIDIOC_ENUM_FRAMESIZES, &s)) {
		if (s.type==V4L2_FRMSIZE_TYPE_DISCRETE) {
			list.push_back(ppl7::grafix::Size(s.discrete.width,
					s.discrete.height));
		}
		s.index++;
	}
	::close(ff);
	*/
}

void Device::open(const VideoDevice &dev)
{
	close();
	if (!(dev.caps&VideoDevice::CAP_VIDEO_CAPTURE)) throw DeviceDoesNotSupportCapture();

}

void Device::close()
{
}

void Device::enumerateControls(std::list<CameraControl> &list)
{

}

void Device::startCapture(const VideoFormat &fmt, int width, int height)
{

}

void Device::startCapture(const VideoFormat &fmt, const ppl7::grafix::Size &size)
{
	startCapture(fmt,size.width,size.height);
}


int Device::readFrame(ppl7::grafix::Image &img)
{
	return 0;
}


void Device::setControlValue(const CameraControl &ctl, int value)
{

}

int Device::getControlValue(const CameraControl &ctl)
{
	return 0;
}



#endif	// #ifdef WIN32


