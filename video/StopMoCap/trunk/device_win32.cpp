#ifdef WIN32
#include "device.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>




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


