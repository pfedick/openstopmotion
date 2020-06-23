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
		uint64_t timeout;
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
