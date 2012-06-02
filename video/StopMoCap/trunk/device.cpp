#include "device.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <string.h>


#define CLEAR(x) memset(&(x), 0, sizeof(x))

static int xioctl(int fh, int request, void *arg)
{
        int r;

        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);

        return r;
}


Device::Device()
{
	myff=0;
}

Device::~Device()
{
	close();
}

void Device::enumerateDevices(std::list<VideoDevice> &list)
{
	ppl7::String DeviceName;
	list.clear();
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
}

void Device::enumerateDevice(const ppl7::String &DeviceName, int index, VideoDevice &d)
{
	int ff=::open((const char*)DeviceName, O_RDONLY);
	if (ff==-1) ppl7::File::throwErrno(errno,DeviceName);
	struct v4l2_input argp;
	argp.index=index;
	if (xioctl( ff, VIDIOC_ENUMINPUT, &argp)) {
		::close(ff);
		throw InvalidDevice();
	}
	d.index=index;
	d.DeviceName=DeviceName;

	struct v4l2_capability cap;
	if (-1==ioctl(ff,VIDIOC_QUERYCAP,&cap)) {
		::close(ff);
		throw InvalidDevice();
	}
	d.Name.setf("%s",cap.card);
	::close(ff);
}

void Device::enumerateImageFormats(std::list<VideoFormat> &list, const VideoDevice &device)
{
	int ff=::open((const char*)device.DeviceName, O_RDONLY);
	if (ff==-1) ppl7::File::throwErrno(errno,device.DeviceName);

	struct v4l2_fmtdesc fmt;
	memset (&fmt, 0, sizeof (fmt));
	fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

	while (0 == xioctl (ff, VIDIOC_ENUM_FMT, &fmt)) {
		//printf ("Standardname: %s\n", fmt.description);
		VideoFormat f;
		f.Description.set((const char*)fmt.description);
		f.pixelformat=fmt.pixelformat;
		f.flags=fmt.flags;
		list.push_back(f);
		fmt.index++;
	}
	::close(ff);
}

void Device::enumerateFrameSizes(std::list<ppl7::grafix::Size> &list, const VideoDevice &device, const VideoFormat &fmt)
{
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
}

void Device::open(const VideoDevice &dev)
{
	close();
	myff=::open((const char*)dev.DeviceName, O_RDONLY);
	if (myff==-1) ppl7::File::throwErrno(errno,dev.DeviceName);

}

void Device::close()
{
	if (myff>0) {
		::close(myff);
		myff=0;
	}
}

void Device::setCaptureFormat(const VideoFormat &fmt, int width, int height)
{
	if (myff<1) throw DeviceNotOpen();
	this->fmt=fmt;
	struct v4l2_format f;
	CLEAR(f);
	f.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	f.fmt.pix.width=width;
	f.fmt.pix.height=height;
	f.fmt.pix.pixelformat=fmt.pixelformat;
	f.fmt.pix.field=V4L2_FIELD_NONE;
	if (-1 == xioctl(myff, VIDIOC_S_FMT, &f)) {
		throw InvalidFormat();
	}
}

void Device::setCaptureFormat(const VideoFormat &fmt, const ppl7::grafix::Size &size)
{
	setCaptureFormat(fmt,size.width,size.height);
}


