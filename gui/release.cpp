/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author: pafe1234@gmail.com $
 * $Revision: 430 $
 * $Date: 2013-04-07 20:57:01 +0200 (So, 07. Apr 2013) $
 * $Id: ledslider.cpp 430 2013-04-07 18:57:01Z pafe1234@gmail.com $
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
#include "release.h"

ReleaseVideo::ReleaseVideo(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);


	open();
}


ReleaseVideo::~ReleaseVideo()
{

}

void ReleaseVideo::setReleaseDir(const ppl7::String &dir)
{
	ReleaseDir=dir;
}

void ReleaseVideo::setReleaseCommand(const ppl7::String &cmd)
{
	ReleaseCommand=cmd;
}

void ReleaseVideo::setScene(const ppl7::String &name)
{
	SceneName=name;
}

void ReleaseVideo::setSourceFrameRate(int fps)
{
	source_fps=fps;
}

void ReleaseVideo::exportVideo(FrameBuffer &buffer)
{
	ppl7::String Tmp, Cmd;
	ui.progressBar->setMinimum(0);
	ui.progressBar->setMaximum(buffer.size());
	ui.progressBar->setValue(0);
	Tmp.setf("%td",buffer.size());
	ui.frameCount->setText(Tmp);
	ui.sceneName->setText(SceneName);

	ppl7::grafix::Image img;
	buffer.copyImage(1,img);


	Cmd=ReleaseCommand;
	Cmd.replace("%width",ppl7::ToString("%i",img.width()));
	Cmd.replace("%height",ppl7::ToString("%i",img.height()));
	Cmd.replace("%frameRate",ppl7::ToString("%i",source_fps));
	Cmd.replace("%releasedir",ReleaseDir);
	Cmd.replace("%scenename",SceneName);

	Cmd.printnl();

	/*
	 * 	if (ReleaseCommand.isEmpty()) ReleaseCommand="ffmpeg -f rawvideo -pix_fmt bgra -s %widthx%height -r %frameRate -i - -vcodec mpeg2video -r 30 "
			"-pix_fmt yuv422p -q:v 1 -qscale 1 -qmin 1 -intra -vb 100M  -b 100M -b:v 100M  -an "
			"%releasedir/%scenename.avi";
	 */
	open();
	QCoreApplication::processEvents();
	QCoreApplication::processEvents();
	QCoreApplication::processEvents();

	ppl7::File pipe;
	pipe.popen(Cmd,ppl7::File::WRITE);
	for (size_t frame=1;frame<=buffer.size();frame++) {
		ui.progressBar->setValue(frame);
		QCoreApplication::processEvents();
		buffer.copyImage(frame,img);
		if (img.adr()) {
			//printf("adr=%td, pitch=%d, height=%d\n",img.adr(),img.pitch(), img.height() );
			pipe.write(img.adr(), img.pitch()*img.height());
		}
	}


	pipe.close();
}


void ReleaseVideo::on_abortButton_clicked()
{
	done(0);
}


