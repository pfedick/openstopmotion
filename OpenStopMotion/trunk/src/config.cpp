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
	saveCamShot=true;
	saveComposited=false;
	frameRate=30;
	interpolate=false;
	overblendFactor=50;
	darkLayout=false;
}

Config::~Config()
{

}

void Config::load()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, STOPMOCAP_ORGANISATION,STOPMOCAP_CONFIGKEY);
	settings.beginGroup("window");
	ScreenGeometry=settings.value("geometry").toByteArray();
	settings.endGroup();

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
	frameRate=settings.value("frameRate",30).toInt();
	interpolate=settings.value("interpolate",false).toBool();
	darkLayout=settings.value("darkLayout",false).toBool();
	overblendFactor=settings.value("overblendFactor",50).toInt();

	jpegQualityComp=settings.value("jpegQualityComp",90).toInt();
	pictureFormatComp=settings.value("pictureFormatComp",0).toInt();

	saveCamShot=settings.value("saveCamShot",true).toBool();
	saveComposited=settings.value("saveComposited",false).toBool();

	scalingMode=settings.value("scalingMode",PPL7ImageViewer::Smooth).toInt();
	settings.endGroup();

	settings.beginGroup("chromaKeying");
	chromaKeyEnabled=settings.value("chromaKeyEnabled",false).toBool();
	chromaBGImage=settings.value("chromaBGImage","").toString();
	chromaToleranceFar=settings.value("chromaToleranceFar",0).toInt();
	chromaToleranceNear=settings.value("chromaToleranceNear",0).toInt();
	chromaKey.setColor((ppluint32)settings.value("chromaKconfey",0x00ff0000).toInt());
	replaceColor.setColor((ppluint32)settings.value("replaceColor",0x00ff0000).toInt());
	chromaReplaceMode=settings.value("chromaReplaceMode",0).toInt();

	foregroundEnabled=settings.value("foregroundEnabled",false).toBool();
	chromaFGImage=settings.value("chromaFGImage","").toString();
	chromaToleranceFarFG=settings.value("chromaToleranceFarFG",0).toInt();
	chromaToleranceNearFG=settings.value("chromaToleranceNearFG",0).toInt();
	chromaKeyFG.setColor((ppluint32)settings.value("chromaKeyFG",0x00ff0000).toInt());

	settings.endGroup();

	settings.beginGroup("ledcontrol");
	LedControlFile=settings.value("LedControlFile").toString();
	ArduinoDevice=settings.value("ArduinoDevice","/dev/cuaU0").toString();
	ArduinoBaudRate=settings.value("ArduinoBaudRate",57600).toInt();
	settings.endGroup();



}

void Config::save()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, STOPMOCAP_ORGANISATION,STOPMOCAP_CONFIGKEY);
	settings.beginGroup("window");
	settings.setValue("geometry",ScreenGeometry);
	settings.endGroup();

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
	settings.setValue("jpegQualityComp",jpegQualityComp);
	settings.setValue("pictureFormatComp",pictureFormatComp);
	settings.setValue("saveCamShot",saveCamShot);
	settings.setValue("saveComposited",saveComposited);
	settings.setValue("frameRate",frameRate);
	settings.setValue("interpolate",interpolate);
	settings.setValue("overblendFactor",overblendFactor);
	settings.setValue("darkLayout",darkLayout);
	settings.endGroup();

	settings.beginGroup("chromaKeying");
	settings.setValue("chromaKeyEnabled",chromaKeyEnabled);
	settings.setValue("chromaBGImage",chromaBGImage);
	settings.setValue("chromaToleranceFar",chromaToleranceFar);
	settings.setValue("chromaToleranceNear",chromaToleranceNear);
	settings.setValue("chromaSpillRemove",chromaSpillRemove);
	settings.setValue("chromaKey",chromaKey.rgb());
	settings.setValue("replaceColor",replaceColor.rgb());
	settings.setValue("chromaReplaceMode",chromaReplaceMode);

	settings.setValue("foregroundEnabled",foregroundEnabled);
	settings.setValue("chromaFGImage",chromaFGImage);
	settings.setValue("chromaToleranceFarFG",chromaToleranceFarFG);
	settings.setValue("chromaToleranceNearFG",chromaToleranceNearFG);
	settings.setValue("chromaSpillRemoveFG",chromaSpillRemoveFG);
	settings.setValue("chromaKeyFG",chromaKeyFG.rgb());
	settings.endGroup();

	settings.beginGroup("ledcontrol");
	settings.setValue("LedControlFile",LedControlFile);
	settings.setValue("ArduinoDevice",ArduinoDevice);
	settings.setValue("ArduinoBaudRate",ArduinoBaudRate);
	settings.endGroup();
}
