#define WITH_QT

#include <ppl7.h>
#include <QSettings>
#include <QDesktopServices>
#include "stopmocap.h"



Config::Config()
{
	mergeFrames=3;
	skipFrames=0;
	onionValue=0;
	scalingMode=PPL7ImageViewer::Smooth;
}

Config::~Config()
{

}

void Config::load()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, STOPMOCAP_ORGANISATION,STOPMOCAP_CONFIGKEY);
	settings.beginGroup("common");
	CaptureDir=settings.value("CaptureDir").toString();
	Scene=settings.value("Scene").toString();
	DeviceName=settings.value("DeviceName").toString();
	ImageFormat=settings.value("ImageFormat").toString();
	ImageSize=settings.value("ImageSize").toString();
	mergeFrames=settings.value("mergeFrames",3).toInt();
	skipFrames=settings.value("skipFrames",0).toInt();
	onionValue=settings.value("onionValue",0).toInt();
	scalingMode=settings.value("scalingMode",PPL7ImageViewer::Smooth).toInt();
	settings.endGroup();
}

void Config::save()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, STOPMOCAP_ORGANISATION,STOPMOCAP_CONFIGKEY);
	settings.beginGroup("common");
	settings.setValue("CaptureDir",CaptureDir);
	settings.setValue("Scene",Scene);
	settings.setValue("DeviceName",DeviceName);
	settings.setValue("ImageFormat",ImageFormat);
	settings.setValue("ImageSize",ImageSize);
	settings.setValue("mergeFrames",mergeFrames);
	settings.setValue("skipFrames",skipFrames);
	settings.setValue("onionValue",onionValue);
	settings.setValue("scalingMode",scalingMode);
	settings.endGroup();
}
