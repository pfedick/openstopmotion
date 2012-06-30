#define WITH_QT

#include <ppl7.h>
#include <ppl7-grafix.h>
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
	jpegQuality=settings.value("jpegQuality",90).toInt();
	pictureFormat=settings.value("pictureFormat",0).toInt();

	scalingMode=settings.value("scalingMode",PPL7ImageViewer::Smooth).toInt();
	settings.endGroup();

	settings.beginGroup("chromaKeying");
	chromaKeyEnabled=settings.value("chromaKeyEnabled",false).toBool();
	chromaBGImage=settings.value("chromaBGImage","").toString();
	chromaToleranceFar=settings.value("chromaToleranceFar",0).toInt();
	chromaToleranceNear=settings.value("chromaToleranceNear",0).toInt();
	chromaKey.setColor((ppluint32)settings.value("chromaKey",0x00ff0000).toInt());
	chromaReplaceColor=settings.value("chromaReplaceColor",0).toInt();
	chromaCaptureMode=settings.value("chromaCaptureMode",0).toInt();
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
	settings.setValue("jpegQuality",jpegQuality);
	settings.setValue("pictureFormat",pictureFormat);
	settings.endGroup();

	settings.beginGroup("chromaKeying");
	settings.setValue("chromaKeyEnabled",chromaKeyEnabled);
	settings.setValue("chromaBGImage",chromaBGImage);
	settings.setValue("chromaToleranceFar",chromaToleranceFar);
	settings.setValue("chromaToleranceNear",chromaToleranceNear);
	settings.setValue("chromaSpillRemove",chromaSpillRemove);
	settings.setValue("chromaKey",chromaKey.rgb());
	settings.setValue("chromaReplaceColor",chromaReplaceColor);
	settings.setValue("chromaCaptureMode",chromaCaptureMode);
	settings.endGroup();
}
