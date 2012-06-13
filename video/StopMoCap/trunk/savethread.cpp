#include "savethread.h"

SaveThread::SaveThread()
{
	timeout=0;
	running=false;
}

SaveThread::~SaveThread()
{

}

void SaveThread::run()
{
	running=true;
	timeout=ppl7::GetTime()+60;
	while (1) {
		signal.wait(2000);
		mutex.lock();
		SaveJob *job=jobs.front();
		if (job) {
			jobs.pop();
			timeout=ppl7::GetTime()+60;
			mutex.unlock();
			executeJob(*job);
			delete job;
		} else {
			if (threadShouldStop()==true || ppl7::GetTime()>timeout) {
				running=false;
				mutex.unlock();
				break;
			}
		}
	}
}

void SaveThread::stop()
{
	threadStop();
}

void SaveThread::addJob(SaveJob *job)
{
	mutex.lock();
	jobs.push(job);
	if (!running) threadStart();
	signal.signal();
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

