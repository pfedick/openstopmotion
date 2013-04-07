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

#include "savethread.h"

//#define DEBUGOUT

SaveThread::SaveThread()
{
	timeout=0;
	running=false;
}

SaveThread::~SaveThread()
{
	signal.signal();
}

void SaveThread::run()
{
	running=true;
#ifdef DEBUGOUT
	ppl7::PrintDebugTime("SaveThread läuft\n");
#endif
	timeout=ppl7::GetTime()+60;
	while (1) {
		mutex.lock();
		if (!jobs.empty()) {
			SaveJob *job=jobs.front();
#ifdef DEBUGOUT
			ppl7::PrintDebugTime("Verarbeite Job\n");
#endif
			jobs.pop();
			timeout=ppl7::GetTime()+60;
			mutex.unlock();
			executeJob(*job);
			delete job;
#ifdef DEBUGOUT
			ppl7::PrintDebugTime("Job beendet\n");
#endif
		} else {
			mutex.unlock();
			if (threadShouldStop()==true || ppl7::GetTime()>timeout) {
				mutex.lock();
				running=false;
				mutex.unlock();
				break;
			}
#ifdef DEBUGOUT
			ppl7::PrintDebugTime("SaveThread wird wartet auf signal\n");
#endif
			signal.wait(10000);
		}
	}
#ifdef DEBUGOUT
	ppl7::PrintDebugTime("SaveThread wird beendet\n");
#endif
}

void SaveThread::stop()
{
	threadShouldStop();
	signal.signal();
	threadStop();
}

void SaveThread::addJob(SaveJob *job)
{
#ifdef DEBUGOUT
	ppl7::PrintDebugTime("Füge SaveJob hinzu\n");
#endif
	mutex.lock();
	jobs.push(job);
	if (!running) {
#ifdef DEBUGOUT
		ppl7::PrintDebugTime("SaveThread wird gestartet\n");
#endif
		threadStart();
	} else {
#ifdef DEBUGOUT
		ppl7::PrintDebugTime("Signalisiere SaveThread\n");
#endif
		signal.signal();
	}
	mutex.unlock();
}

void SaveThread::executeJob(const SaveJob &job) const
{
	if (job.format==PictureFormat::png) {
		ppl7::grafix::ImageFilter_PNG png;
		png.saveFile(job.Filename,job.img);
	} else 	if (job.format==PictureFormat::bmp) {
		ppl7::grafix::ImageFilter_BMP bmp;
		bmp.saveFile(job.Filename,job.img);
	} else 	if (job.format==PictureFormat::jpeg) {
		ppl7::grafix::ImageFilter_JPEG jpg;
		ppl7::AssocArray param;
		param.setf("quality","%i",job.quality);
		jpg.saveFile(job.Filename,job.img,param);
	}
}

