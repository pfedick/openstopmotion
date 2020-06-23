/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick <patrick@pfp.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define WITH_QT

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <QSettings>
#include <QDesktopServices>
#include "osm.h"
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
	MotionControlPort=0;
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

	settings.beginGroup("release");
	ReleaseDir=settings.value("ReleaseDir").toString();
	ReleaseCommand=settings.value("ReleaseCommand").toString();

	if (ReleaseDir.isEmpty()) ReleaseDir="%captureDir%/../Video";
	if (ReleaseCommand.isEmpty()) ReleaseCommand="ffmpeg -f rawvideo -pix_fmt bgra -s %widthx%height -r %frameRate -i - -vcodec mpeg2video -r 30 "
			"-pix_fmt yuv422p -q:v 1 -qscale 1 -qmin 1 -intra -vb 100M  -b 100M -b:v 100M  -an -y "
			"\"%releasedir/%scenename.m2v\"";

	settings.endGroup();

	settings.beginGroup("chromaKeying");
	chromaKeyEnabled=settings.value("chromaKeyEnabled",false).toBool();
	chromaBGImage=settings.value("chromaBGImage","").toString();
	chromaToleranceFar=settings.value("chromaToleranceFar",0).toInt();
	chromaToleranceNear=settings.value("chromaToleranceNear",0).toInt();
	chromaKey.setColor((uint32_t)settings.value("chromaKconfey",0x00ff0000).toInt());
	replaceColor.setColor((uint32_t)settings.value("replaceColor",0x00ff0000).toInt());
	chromaReplaceMode=settings.value("chromaReplaceMode",0).toInt();

	foregroundEnabled=settings.value("foregroundEnabled",false).toBool();
	chromaFGImage=settings.value("chromaFGImage","").toString();
	chromaToleranceFarFG=settings.value("chromaToleranceFarFG",0).toInt();
	chromaToleranceNearFG=settings.value("chromaToleranceNearFG",0).toInt();
	chromaKeyFG.setColor((uint32_t)settings.value("chromaKeyFG",0x00ff0000).toInt());

	settings.endGroup();

	settings.beginGroup("ledcontrol");
	LedControlFile=settings.value("LedControlFile").toString();
	ArduinoDevice=settings.value("ArduinoDevice","/dev/cuaU0").toString();
	ArduinoBaudRate=settings.value("ArduinoBaudRate",57600).toInt();
	settings.endGroup();

	settings.beginGroup("motioncontrol");
	MotionControlHostname=settings.value("MotionControlHostname").toString();
	MotionControlPort=settings.value("MotionControlPort").toInt();
	MotionControlFile=settings.value("MotionControlFile").toString();
	MotionControlMoveSteps=settings.value("MotionControlMoveSteps",100).toInt();
	MotionControlTurnSteps=settings.value("MotionControlTurnSteps",100).toInt();
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

	settings.beginGroup("release");
	settings.setValue("ReleaseDir",ReleaseDir);
	settings.setValue("ReleaseCommand",ReleaseCommand);
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

	settings.beginGroup("motioncontrol");
	settings.setValue("MotionControlHostname",MotionControlHostname);
	settings.setValue("MotionControlPort",MotionControlPort);
	settings.setValue("MotionControlFile",MotionControlFile);
	settings.setValue("MotionControlMoveSteps",MotionControlMoveSteps);
	settings.setValue("MotionControlTurnSteps",MotionControlTurnSteps);
	settings.endGroup();

}
