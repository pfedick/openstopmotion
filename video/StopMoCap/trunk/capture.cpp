#include "capture.h"

Capture::Capture()
{

}


Capture::~Capture()
{

}

void Capture::enumerate(ppl7::List<CaptureDevice> &list)
{
	list.clear();
	wchar_t bufferName[256];
	wchar_t bufferVersion[256];
	for (int i=0;i<9;i++) {
		if (capGetDriverDescription(i,bufferName,255,bufferVersion,255)) {
			CaptureDevice dev;
			dev.Name.set(bufferName);
			dev.Version.set(bufferVersion);
			dev.Id=i;
			list.add(dev);
		}
	}
}
