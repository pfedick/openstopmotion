#include "scenemanager.h"

SceneManager::SceneManager()
{
	highest=0;
}

SceneManager::~SceneManager()
{

}

void SceneManager::clear()
{
	frames.clear();
	highest=0;
}

void SceneManager::loadScene(const ppl7::String &path)
{
	clear();
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
				frames.insert(id,f);
			}
		}
	} catch (...) {

	}
}
