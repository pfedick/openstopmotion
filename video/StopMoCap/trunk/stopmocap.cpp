#define WITH_QT
#include <ppl7.h>
#include "stopmocap.h"
#include "capture.h"

StopMoCap::StopMoCap(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	ppl7::List<CaptureDevice> devices;
	ppl7::List<CaptureDevice>::Iterator itDevices;

	cap.enumerate(devices);
	ui.deviceComboBox->clear();
	devices.reset(itDevices);
	while (devices.getNext(itDevices)) {
		ui.deviceComboBox->addItem(itDevices.value().Name);
	}

}

StopMoCap::~StopMoCap()
{

}
