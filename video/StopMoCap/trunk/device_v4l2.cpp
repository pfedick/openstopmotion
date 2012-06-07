#ifndef WIN32
#include "device.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
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
	n_buffers=0;
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
	d.caps=0;

	struct v4l2_capability cap;
	if (-1==ioctl(ff,VIDIOC_QUERYCAP,&cap)) {
		::close(ff);
		throw InvalidDevice();
	}
	d.Name.setf("%s",cap.card);

	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) d.caps|=VideoDevice::CAP_VIDEO_CAPTURE;
    if ((cap.capabilities & V4L2_CAP_READWRITE)) d.caps|=VideoDevice::CAP_READWRITE;
    if ((cap.capabilities & V4L2_CAP_STREAMING)) d.caps|=VideoDevice::CAP_STREAMING;
    if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY)) d.caps|=VideoDevice::CAP_VIDEO_OUTPUT_OVERLAY;
    if ((cap.capabilities & V4L2_CAP_ASYNCIO)) d.caps|=VideoDevice::CAP_ASYNCIO;
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
	if (!(dev.caps&VideoDevice::CAP_VIDEO_CAPTURE)) throw DeviceDoesNotSupportCapture();
	myff=::open((const char*)dev.DeviceName, O_RDWR /* required */ | O_NONBLOCK,0);
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

void Device::initBuffers(int n, size_t buffer_size)
{
	if (n<1) throw ppl7::IllegalArgumentException();
	n_buffers=n;
	buffers = (struct buffer *)calloc(n, sizeof(*buffers));

	if (!buffers) throw ppl7::OutOfMemoryException();

	if (iomethod==IO_METHOD_MMAP) {
		for (unsigned int i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;
			CLEAR(buf);
			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = i;
			if (-1 == xioctl(myff, VIDIOC_QUERYBUF, &buf)) throw QueryBufFailed();

			buffers[i].length = buf.length;
			buffers[i].start = mmap(NULL /* start anywhere */,
	                buf.length,
	                PROT_READ | PROT_WRITE /* required */,
	                MAP_SHARED /* recommended */,
	                myff, buf.m.offset);

			if (MAP_FAILED == buffers[i].start) {
				perror("MMap failed");
				buffers[i].start=NULL;
				freeBuffers();
				throw MMapFailed();
			}
		}
	} else {
		for (int i = 0; i < n; ++i) {
			buffers[i].length = buffer_size;
			buffers[i].start = malloc(buffer_size);
			if (!buffers[i].start) {
				freeBuffers();
				throw ppl7::OutOfMemoryException();
			}
		}
	}
}

void Device::freeBuffers()
{
	if (buffers) {
		if (iomethod==IO_METHOD_MMAP) {
			for (unsigned int i = 0; i < n_buffers; ++i) {
				if (buffers[i].start!=NULL && buffers[i].length>0) {
					if (-1 == munmap(buffers[i].start, buffers[i].length)) throw MUnmapFailed();
				}
			}

		} else {
			for (unsigned int i = 0; i < n_buffers; ++i) free(buffers[i].start);
		}
		free(buffers);
		buffers=NULL;
	}
}

void Device::enumerateControls(std::list<CameraControl> &list)
{
	struct v4l2_queryctrl qctrl;
	struct v4l2_querymenu menu;

	//qctrl.id = V4L2_CTRL_CLASS_CAMERA | V4L2_CTRL_FLAG_NEXT_CTRL;
	qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;

	while (0 == xioctl (myff, VIDIOC_QUERYCTRL, &qctrl)) {
		//if (V4L2_CTRL_ID2CLASS (qctrl.id) != V4L2_CTRL_CLASS_CAMERA) break;
		if (qctrl.flags&V4L2_CTRL_FLAG_DISABLED) {
			qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
			continue;
		}
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
		printf ("id: %x, type: %i, Name: %s, min: %i, max: %i, step: %i, default: %i, flags: %i\n",
				cc.id,cc.type,(const char*)cc.Name,cc.min,cc.max,
				cc.step, cc.defaultValue,cc.flags);
		printf ("  currentValue: %i\n",getControlValue(cc));
		*/
		if (qctrl.type==V4L2_CTRL_TYPE_MENU) {
			CLEAR(menu);
			menu.id=qctrl.id;
			//printf ("Enumerating Menu...\n");
			for (menu.index=(unsigned int)qctrl.minimum;menu.index<=(unsigned int)qctrl.maximum;menu.index++) {
				if (0 == xioctl (myff, VIDIOC_QUERYMENU, &menu)) {
					//printf ("Menue: %i, %s\n",menu.index,(const char*)menu.name);
					cc.MenuItems.push_back(CameraControl::MenuItem(menu.index,(const char*)menu.name));
				} else {
					//printf ("Error\n");
				}
			}
		}
		list.push_back(cc);
		qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
	}
}

void Device::startCapture(const VideoFormat &fmt, int width, int height)
{
	if (myff<1) throw DeviceNotOpen();
	if (captureRunning) stopCapturing();
	this->fmt=fmt;

	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 == xioctl(myff, VIDIOC_CROPCAP, &cropcap)) {
    	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    	crop.c = cropcap.defrect; /* reset to default */
    	if (-1 == xioctl(myff, VIDIOC_S_CROP, &crop)) {
    		switch (errno) {
    			case EINVAL:
    				/* Cropping not supported. */
    				break;
    			default:
    				/* Errors ignored. */
    				break;
    		}
    	}
    } else {
    	/* Errors ignored. */
    }



	struct v4l2_format f;
	CLEAR(f);
	f.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	f.fmt.pix.width=width;
	f.fmt.pix.height=height;
	f.fmt.pix.pixelformat=fmt.pixelformat;
	f.fmt.pix.field=V4L2_FIELD_NONE;
	//f.fmt.pix.pixelformat=V4L2_PIX_FMT_RGB24;
	//f.fmt.pix.field       = V4L2_FIELD_INTERLACED;


	if (-1 == xioctl(myff, VIDIOC_S_FMT, &f)) {
		throw InvalidFormat();
	}
	/*
	if ((int)f.fmt.pix.pixelformat!=fmt.pixelformat) {
		throw InvalidFormat();
	}
	*/

	initCapture(width*height*4);
	startCapturing();

}

void Device::startCapture(const VideoFormat &fmt, const ppl7::grafix::Size &size)
{
	startCapture(fmt,size.width,size.height);
}


void Device::initMMap()
{
	struct v4l2_requestbuffers req;
	CLEAR(req);
	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (-1 == xioctl(myff, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			throw MMapUnsupported();
		} else {
			throw StreamingUnsupported();
		}
	}
	if (req.count < 2) throw InsufficientBufferMemory();
	iomethod=IO_METHOD_MMAP;
	initBuffers(4,0);
}

void Device::initUserp(size_t buffer_size)
{
	struct v4l2_requestbuffers req;
	CLEAR(req);
	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;
	if (-1 == xioctl(myff, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			throw UserpUnsupported();
		} else {
			throw StreamingUnsupported();
		}
	}
	iomethod=IO_METHOD_USERPTR;
	initBuffers(4,buffer_size);
}


void Device::initCapture(size_t buffer_size)
{
	if (myff<1) throw DeviceNotOpen();
	freeBuffers();
	if (dev.caps&VideoDevice::CAP_STREAMING) {
		try {
			initMMap();
			return;
		} catch (MMapUnsupported &) {
			// Wir machen erstmal nichts, vielleicht geht ja Userp
		}
		try {
			initUserp(buffer_size);
			return;
		} catch (MMapUnsupported &) {
			// Wir machen erstmal nichts, vielleicht geht ja Readwrite
		}
	}
	if (dev.caps&VideoDevice::CAP_READWRITE) {
		initBuffers(1,buffer_size);
		iomethod=IO_METHOD_READ;
		return;
	}
	throw DeviceDoesNotSupportCapture();
}

void Device::stopCapturing()
{
	enum v4l2_buf_type type;
	switch (iomethod) {
        case IO_METHOD_READ:
                /* Nothing to do. */
                break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(myff, VIDIOC_STREAMOFF, &type)) throw StreamOffFailed();
                break;
	}
	captureRunning=false;
}

void Device::startCapturing()
{
	unsigned int i;
	enum v4l2_buf_type type;
	switch (iomethod) {
		case IO_METHOD_READ:
			/* Nothing to do. */
			break;

		case IO_METHOD_MMAP:
			for (i = 0; i < n_buffers; ++i) {
				struct v4l2_buffer buf;
				CLEAR(buf);
				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_MMAP;
				buf.index = i;

				if (-1 == xioctl(myff, VIDIOC_QBUF, &buf)) throw QueryBufFailed();
			}
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl(myff, VIDIOC_STREAMON, &type)) throw StreamOnFailed();
			break;

		case IO_METHOD_USERPTR:
			for (i = 0; i < n_buffers; ++i) {
				struct v4l2_buffer buf;

				CLEAR(buf);
				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_USERPTR;
				buf.index = i;
				buf.m.userptr = (unsigned long)buffers[i].start;
				buf.length = buffers[i].length;

				if (-1 == xioctl(myff, VIDIOC_QBUF, &buf)) throw QueryBufFailed();
			}
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl(myff, VIDIOC_STREAMON, &type)) throw StreamOnFailed();
			break;
	}
	captureRunning=true;
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

int Device::readFrame(ppl7::grafix::Image &img)
{
	if (myff<1) throw DeviceNotOpen();
	waitForNextFrame();
	struct v4l2_buffer buf;
	unsigned int i;

	switch (iomethod) {
		case IO_METHOD_READ:
			if (-1 == ::read(myff, buffers[0].start, buffers[0].length)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						/* Could ignore EIO, see spec. */

						/* fall through */

					default:
						throw ReadError();
				}
			}
			processImage(buffers[0].start, buffers[0].length, img);
			break;

		case IO_METHOD_MMAP:
			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;

			if (-1 == xioctl(myff, VIDIOC_DQBUF, &buf)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						/* Could ignore EIO, see spec. */

						/* fall through */

					default:
						throw ReadError();
				}
			}
			if (buf.index>=n_buffers) throw BufferError();

			processImage(buffers[buf.index].start, buf.bytesused, img);

			if (-1 == xioctl(myff, VIDIOC_QBUF, &buf)) throw QueryBufFailed();
			break;

		case IO_METHOD_USERPTR:
			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;

			if (-1 == xioctl(myff, VIDIOC_DQBUF, &buf)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						/* Could ignore EIO, see spec. */

						/* fall through */

					default:
						throw ReadError();
				}
			}

			for (i = 0; i < n_buffers; ++i)
				if (buf.m.userptr == (unsigned long)buffers[i].start
						&& buf.length == buffers[i].length)
					break;

			if (i>=n_buffers) throw BufferError();

			processImage((void *)buf.m.userptr, buf.bytesused, img);

			if (-1 == xioctl(myff, VIDIOC_QBUF, &buf)) throw QueryBufFailed();
			break;
	}

	return 1;
}


void Device::setControlValue(const CameraControl &ctl, int value)
{
	if (myff<1) throw DeviceNotOpen();
	struct v4l2_control c;
	CLEAR(c);
	printf ("Setting Control %x (%s) to %i\n",ctl.id,(const char*)ctl.Name,value);
	c.id=ctl.id;
	c.value=value;
	if (-1 == xioctl(myff, VIDIOC_S_CTRL, &c)) {
		perror("ERROR Device::setControlValue");
		//throw SetControlValueFailed();
	}
}

int Device::getControlValue(const CameraControl &ctl)
{
	if (myff<1) throw DeviceNotOpen();
	struct v4l2_control c;
	CLEAR(c);
	//ctls.ctrl_class=V4L2_CTRL_CLASS_CAMERA;
	c.id=ctl.id;
	if (-1 == xioctl(myff, VIDIOC_G_CTRL, &c)) {
		//perror("ERROR Device::getControlValue");
		//throw GetControlValueFailed();
		return 0;
	}
	//ppl7::HexDump(&c,sizeof(c));
	return c.value;
}


void Device::processImage(void *buffer, size_t size, ppl7::grafix::Image &img)
{
	//printf ("Frame captured mit %i Bytes. ",size);
	//printf ("Format: %i\n",fmt.pixelformat);
	//ppl7::HexDump(&fmt.pixelformat,4);
	if (fmt.pixelformat==V4L2_PIX_FMT_JPEG || fmt.pixelformat==V4L2_PIX_FMT_MJPEG) {
		ppl7::MemFile File;
		File.open(buffer,size);
		img.load(File,ppl7::grafix::RGBFormat::X8R8G8B8);


	}

	//ppl7::HexDump(buffer,256);
}

#endif	// #ifndef WIN32


