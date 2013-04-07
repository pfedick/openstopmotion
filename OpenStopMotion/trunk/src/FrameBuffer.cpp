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


#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
	// TODO Auto-generated constructor stub
	ppl7::SetGlobalOutput(0);
}

FrameBuffer::~FrameBuffer()
{
	// TODO Auto-generated destructor stub

}

void FrameBuffer::pushBack(const ppl7::grafix::Drawable &img)
{
	//ppl7::PrintDebugTime("FrameBuffer::pushBack()\n");
	mutex.lock();
	Frames.insert ( std::pair<int,ppl7::grafix::Image>(Frames.size(),img) );
	mutex.unlock();
}

void FrameBuffer::setImage(size_t nr, const ppl7::grafix::Drawable &img)
{
	//ppl7::PrintDebugTime("FrameBuffer::setImage(nr=%zd)\n",nr);
	mutex.lock();
	ppl7::grafix::Image &myImg=Frames[nr-1];
	myImg.copy(img);
	mutex.unlock();
}

const ppl7::grafix::Drawable &FrameBuffer::getImage(size_t nr)
{
	//ppl7::PrintDebugTime("FrameBuffer::getImage(nr=%zd)\n",nr-1);
	const ppl7::grafix::Image &cache=Frames[nr-1];
	if (cache.isEmpty()) loadNow(nr);

	return Frames[nr-1];
}

void FrameBuffer::copyImage(size_t nr, ppl7::grafix::Image &img)
{
	//ppl7::PrintDebugTime("FrameBuffer::copyImage(nr=%zd)\n",nr);
	mutex.lock();
	const ppl7::grafix::Image &cache=Frames[nr-1];
	if (cache.isEmpty()) {
		mutex.unlock();
		loadNow(nr);
		mutex.lock();
	}
	img.copy(Frames[nr-1]);
	mutex.unlock();
}

size_t FrameBuffer::size()
{
	size_t s;
	mutex.lock();
	s=Frames.size();
	//ppl7::PrintDebugTime("FrameBuffer::size()=%zd\n",s);
	mutex.unlock();
	return s;
}

void FrameBuffer::popBack()
{
	//ppl7::PrintDebugTime("FrameBuffer::popBack()\n");
	mutex.lock();
	if (Frames.size()>0) {
		std::map<int,ppl7::grafix::Image>::iterator it;
		it=Frames.find ( Frames.size()-1 );
		if (it!=Frames.end()) Frames.erase(it);
	}
	mutex.unlock();
}

void FrameBuffer::loadAsync(const ppl7::String &Dir)
{
	//ppl7::PrintDebugTime("FrameBuffer::loadAsync(): %s\n",(const char*)Dir);
	this->threadStop();
	mutex.lock();
	Frames.clear();
	this->Path=Dir;
	this->threadStart();
	mutex.unlock();
}

void FrameBuffer::clear()
{
	//ppl7::PrintDebugTime("FrameBuffer::clear()\n");
	this->threadStop();
	mutex.lock();
	Frames.clear();
	mutex.unlock();
}

void FrameBuffer::run()
{
	//ppl7::PrintDebugTime("FrameBuffer::run() started\n");
	ppl7::Dir dir;
	try {
		dir.open(Path);
	} catch (...) {
		return;
	}
	ppl7::DirEntry e;
	ppl7::Dir::Iterator it;
	ppl7::Array matches;
	dir.reset(it);
	while (dir.getNextRegExp(e,it,"/^frame_[0-9]+\\..*$/")) {
		if (this->threadShouldStop()) break;
		if (e.Filename.pregMatch("/^frame_0*([1-9]+[0-9]*)\\..*$/",matches)) {
			int id=matches[1].toInt()-1;
			mutex.lock();
			//ppl7::PrintDebugTime("FrameBuffer::run(), loading Frame: %s\n",(const char*)e.File);
			try {
				ppl7::grafix::Image &img=Frames[id];
				img.load(e.File);
			} catch (...) {

			}
			mutex.unlock();
		}
	}
	//ppl7::PrintDebugTime("FrameBuffer::run() ended\n");
}

void FrameBuffer::loadNow(size_t nr)
{
	ppl7::String Filename;
	Filename=Path;
	Filename.appendf("/frame_%06zu.png",nr);
	ppl7::grafix::Image img;
	try {
		img.load(Filename);
		mutex.lock();
		ppl7::grafix::Image &cache=Frames[nr-1];
		cache.copy(img);
		mutex.unlock();
	} catch (...) {

	}
}
