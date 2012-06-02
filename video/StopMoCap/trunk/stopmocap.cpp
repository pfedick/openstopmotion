#define WITH_QT
#include <ppl7.h>
#include "stopmocap.h"
#include "device.h"

StopMoCap::StopMoCap(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	std::list<VideoDevice> devices;
	std::list<VideoDevice>::const_iterator it;

	cam.enumerateDevices(devices);
	ui.deviceComboBox->clear();
	for (it=devices.begin();it!=devices.end();it++) {
		Devices.push_back(*it);
		ui.deviceComboBox->addItem(it->Name,Devices.size()-1);
	}
	ui.deviceComboBox->setCurrentIndex(0);

}

StopMoCap::~StopMoCap()
{

}


void StopMoCap::on_deviceComboBox_currentIndexChanged(int index)
{
	Formats.clear();
	std::list<VideoFormat> fmt;
	std::list<VideoFormat>::const_iterator it;
	ui.formatComboBox->clear();
	cam.enumerateImageFormats(fmt,Devices[index]);
	for (it=fmt.begin();it!=fmt.end();it++) {
		Formats.push_back(*it);
		ui.formatComboBox->addItem(it->Description,Formats.size()-1);
	}

}

void StopMoCap::on_formatComboBox_currentIndexChanged(int index)
{
	ppl7::String Tmp;
	FrameSizes.clear();
	std::list<ppl7::grafix::Size> sizes;
	std::list<ppl7::grafix::Size>::const_iterator it;
	ui.resolutionComboBox->clear();
	cam.enumerateFrameSizes(sizes,
			Devices[ui.deviceComboBox->currentIndex()],
			Formats[index]);
	for (it=sizes.begin();it!=sizes.end();it++) {
		FrameSizes.push_back(*it);
		Tmp.setf("%i x %i",it->width, it->height);
		ui.resolutionComboBox->addItem(Tmp,FrameSizes.size()-1);
	}
}

void StopMoCap::on_useDevice_clicked()
{
	cam.open(Devices[ui.deviceComboBox->currentIndex()]);
	cam.setCaptureFormat(Formats[ui.formatComboBox->currentIndex()],
			FrameSizes[ui.resolutionComboBox->currentIndex()]);



}
