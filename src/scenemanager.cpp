#include "scenemanager.h"


void SceneManager::LoaderThread::run()
{
	ppl7::PrintDebugTime("SceneManager::LoaderThread::run start\n");
	ppl7::String Filename;
	ppl7::grafix::Image img;
	for (size_t i=1;i<manager->getHighestFrameNum();i++) {
		if (this->threadShouldStop()) return;
		Filename=manager->getFilename(i);
		ppl7::PrintDebugTime("SceneManager::LoaderThread::run lade Frame %i: %s\n",i,(const char*)Filename);
		if (Filename.notEmpty()) {
			try {
				img.load(Filename);
				manager->setImage(i,img);
			} catch (...) {

			}
		}
	}
	ppl7::PrintDebugTime("SceneManager::LoaderThread::run end\n");
}


SceneManager::SceneManager()
{
	highest=0;
}

SceneManager::~SceneManager()
{

}

void SceneManager::clear()
{
	lthread.manager=this;
	lthread.threadStop();
	mutex.lock();
	frames.clear();
	highest=0;
	path.clear();
	mutex.unlock();
}

void SceneManager::loadScene(const ppl7::String &path)
{
	clear();
	mutex.lock();
	this->path=path;
	try {
		ppl7::Dir dir(path);
		ppl7::DirEntry e;
		ppl7::Dir::Iterator it;
		ppl7::Array matches;
		dir.reset(it);
		while (dir.getNextRegExp(e,it,"/^frame_[0-9]+\\..*$/")) {
			if (e.Filename.pregMatch("/^frame_0*([1-9]+[0-9]*)\\..*$/",matches)) {
				size_t id=matches[1].toInt();
				if (id>highest) highest=id;
				Frame f;
				f.Filename=e.File;
				frames.insert (std::pair<size_t,Frame>(id,f));
			}
		}
	} catch (...) {

	}
	mutex.unlock();
	lthread.threadStart();
}

void SceneManager::setFilename(size_t nr, const ppl7::String Filename)
{
	mutex.lock();
	Frame &frame=frames[nr];
	frame.Filename=Filename;
	if (nr>highest) highest=nr;
	mutex.unlock();
}

void SceneManager::setImage(size_t nr, const ppl7::grafix::Image &img)
{
	mutex.lock();
	try {
		Frame &frame=frames[nr];
		if (nr>highest) highest=nr;
		ppl7::grafix::ImageFilter_JPEG jpeg;
		ppl7::AssocArray param;
		param.set("quality","60");
		frame.file.rewind();
		jpeg.save(img,frame.file,param);
		frame.file.truncate(frame.file.size());
	} catch (...) {
		mutex.unlock();
		throw;
	}
	mutex.unlock();
}

ppl7::String SceneManager::getFilename(size_t nr)
{
	ppl7::String ret;
	mutex.lock();
	Frame &frame=frames[nr];
	ret=frame.Filename;
	mutex.unlock();
	return ret;
}

void SceneManager::getImage(size_t nr, ppl7::grafix::Image &img)
{
	mutex.lock();
	try {
		Frame &frame=frames[nr];
		if (frame.file.size()==0) {
			img.clear();
		} else {
			img.load(frame.file);
		}
	} catch (...) {
		mutex.unlock();
		throw;
	}
	mutex.unlock();
}

size_t SceneManager::getHighestFrameNum()
{
	size_t ret;
	mutex.lock();
	ret=highest;
	mutex.unlock();
	return ret;
}

void SceneManager::deleteFrame(size_t nr)
{
	mutex.lock();
	if (nr==highest) nr=highest-1;
	std::map<size_t,Frame>::iterator it;
	it=frames.find(nr);
	if (it!=frames.end()) {
		frames.erase(it);
	}
	mutex.unlock();
}

