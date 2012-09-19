/*
 * savethread.h
 *
 *  Created on: 13.06.2012
 *      Author: patrickf
 */

#ifndef SAVETHREAD_H_
#define SAVETHREAD_H_

#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <queue>

namespace PictureFormat {
enum Format
{
	png=0,
	bmp=1,
	jpeg=2
};
};

class SaveJob
{
	public:
		ppl7::grafix::Image img;
		ppl7::String		Filename;
		int format;
		int quality;
};

class SaveThread : ppl7::Thread
{
	private:
		ppl7::Mutex mutex;
		ppl7::Mutex signal;
		std::queue<SaveJob*> jobs;
		ppluint64 timeout;
		bool running;


	public:
		SaveThread();
		~SaveThread();

		void addJob(SaveJob *job);
		void run();
		void stop();
		void executeJob(const SaveJob &job) const;
};


#endif /* SAVETHREAD_H_ */
