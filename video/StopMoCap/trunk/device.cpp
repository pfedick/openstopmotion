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
	buffers=NULL;
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

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        printf ("is no video capture device\n");
	}
    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
    	printf ("%s: Kein readwrite\n",(const char*)d.Name);
    }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    	printf ("%s: Kein Streaming\n",(const char*)d.Name);
    }
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
	if (captureRunning) {
		stopCapturing();
	}
	if (myff>0) {
		::close(myff);
		myff=0;
	}
	freeBuffers();
}

void Device::freeBuffers()
{
	if (buffers) {
        for (int i = 0; i < 4; ++i)
                free(buffers[i].start);
		free(buffers);
		buffers=NULL;
	}
}

void Device::startCapture(const VideoFormat &fmt, int width, int height)
{
	if (myff<1) throw DeviceNotOpen();
	if (captureRunning) stopCapturing();
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
	if ((int)f.fmt.pix.pixelformat!=fmt.pixelformat) {
		throw InvalidFormat();
	}
	initCapture(width*height*4);
}

void Device::startCapture(const VideoFormat &fmt, const ppl7::grafix::Size &size)
{
	startCapture(fmt,size.width,size.height);
}

void Device::enumerateControls(std::list<CameraControl> &list)
{
	struct v4l2_queryctrl qctrl;
	struct v4l2_querymenu menu;

	//qctrl.id = V4L2_CTRL_CLASS_CAMERA | V4L2_CTRL_FLAG_NEXT_CTRL;
	qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;

	while (0 == xioctl (myff, VIDIOC_QUERYCTRL, &qctrl)) {
		qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		//if (V4L2_CTRL_ID2CLASS (qctrl.id) != V4L2_CTRL_CLASS_CAMERA) break;
		if (qctrl.flags&V4L2_CTRL_FLAG_DISABLED) continue;
		CameraControl cc;
		cc.id=qctrl.id;
		switch (qctrl.type) {
			case V4L2_CTRL_TYPE_INTEGER: cc.type=CameraControl::Integer; break;
			case V4L2_CTRL_TYPE_BOOLEAN: cc.type=CameraControl::Boolean; break;
			case V4L2_CTRL_TYPE_MENU: cc.type=CameraControl::Menu; break;
			default:
				cc.type=CameraControl::Unknown; break;
		}
		cc.Name.set((const char*)qctrl.name);
		cc.min=qctrl.minimum;
		cc.max=qctrl.maximum;
		cc.step=qctrl.step;
		cc.defaultValue=qctrl.default_value;
		cc.flags=qctrl.flags;
		/*
		printf ("id: %i, type: %i, Name: %s, min: %i, max: %i, step: %i, default: %i, flags: %i\n",
				cc.id,cc.type,(const char*)cc.Name,cc.min,cc.max,
				cc.step, cc.defaultValue,cc.flags);
		 */
		if (qctrl.type==V4L2_CTRL_TYPE_MENU) {
			CLEAR(menu);
			menu.id=qctrl.id;
			//printf ("Enumerating Menu...\n");
			for (menu.index=(unsigned int)qctrl.minimum;menu.index<=(unsigned int)qctrl.maximum;menu.index++) {
				if (0 == xioctl (myff, VIDIOC_QUERYMENU, &menu)) {
					//printf ("Menue: %i, %s\n",menu.index,(const char*)menu.name);
				} else {
					//printf ("Error\n");
				}
			}
		}
		list.push_back(cc);

	}

}

void Device::initCapture(size_t buffer_size)
{
	if (myff<1) throw DeviceNotOpen();
	struct v4l2_requestbuffers req;
	CLEAR(req);
	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(myff, VIDIOC_REQBUFS, &req)) {
		perror("ERROR");
		if (EINVAL == errno) {
			throw StreamingUnsupported();
		} else {
			throw StreamingUnsupported();
		}
	}
	freeBuffers();
	buffers = (struct buffer *)calloc(4, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (int n_buffers = 0; n_buffers < 4; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = malloc(buffer_size);

		if (!buffers[n_buffers].start) {
			throw ppl7::OutOfMemoryException();
		}
	}
}

void Device::stopCapturing()
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(myff, VIDIOC_STREAMOFF, &type))
		throw StreamOffError();
}

void Device::startCapturing()
{
	unsigned int i;
	enum v4l2_buf_type type;
	for (i = 0; i < 4; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.index = i;
		buf.m.userptr = (unsigned long)buffers[i].start;
		buf.length = buffers[i].length;

		if (-1 == xioctl(myff, VIDIOC_QBUF, &buf)) throw StreamBufferError();
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(myff, VIDIOC_STREAMON, &type)) throw StreamOnError();
}


void Device::waitForNextFrame()
{
	for (;;) {
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(myff, &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		r = select(myff + 1, &fds, NULL, NULL, &tv);

		if (-1 == r) {
			if (EINTR == errno)
				continue;
			throw SelectError();
		}

		if (0 == r) {
			throw Timeout();
		}
		return;
	}
}

void Device::readFrame(ppl7::ByteArray &ba)
{
	if (myff<1) throw DeviceNotOpen();
	waitForNextFrame();
}


