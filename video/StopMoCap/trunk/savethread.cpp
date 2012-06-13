#include "savethread.h"

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
	ppl7::PrintDebugTime("SaveThread läuft\n");
	timeout=ppl7::GetTime()+60;
	while (1) {
		mutex.lock();
		if (!jobs.empty()) {
			SaveJob *job=jobs.front();
			ppl7::PrintDebugTime("Verarbeite Job\n");
			jobs.pop();
			timeout=ppl7::GetTime()+60;
			mutex.unlock();
			executeJob(*job);
			delete job;
			ppl7::PrintDebugTime("Job beendet\n");
		} else {
			if (threadShouldStop()==true || ppl7::GetTime()>timeout) {
				running=false;
				mutex.unlock();
				ppl7::PrintDebugTime("SaveThread wird wartet auf signal\n");
				signal.wait(5000);
				break;
			}
		}
	}
	ppl7::PrintDebugTime("SaveThread wird beendet\n");
}

void SaveThread::stop()
{
	threadShouldStop();
	signal.signal();
	threadStop();
}

void SaveThread::addJob(SaveJob *job)
{
	ppl7::PrintDebugTime("Füge SaveJob hinzu\n");
	mutex.lock();
	jobs.push(job);
	if (!running) {
		ppl7::PrintDebugTime("SaveThread wird gestartet\n");
		threadStart();
	} else {
		ppl7::PrintDebugTime("Signalisiere SaveThread\n");
		signal.signal();
	}
	mutex.unlock();
}

void SaveThread::executeJob(const SaveJob &job)
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

