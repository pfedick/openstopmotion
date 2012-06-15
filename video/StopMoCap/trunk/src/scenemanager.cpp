#include "scenemanager.h"

SceneManager::Frame::Frame()
{
	img=NULL;
}

SceneManager::Frame::~Frame()
{
	if (img) delete img;
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
	frames.clear();
	highest=0;
	path.clear();
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
				frames.insert (std::pair<size_t,Frame>(id,f));
			}
		}
	} catch (...) {

	}
}

void SceneManager::setFilename(size_t nr, const ppl7::String Filename)
{
	Frame f;
	f.Filename=Filename;
	frames.insert (std::pair<size_t,Frame>(nr,f));
	if (nr>highest) highest=nr;

}

void SceneManager::setImage(size_t nr, const ppl7::grafix::Image &img)
{
	std::map<size_t,Frame>::iterator it;
	it=frames.find(nr);
	if (it==frames.end()) return;

	Frame &frame=it->second;
	if (!frame.img) {
		frame.img=new ppl7::ByteArray;
	}
	ppl7::grafix::ImageFilter_JPEG jpeg;



}

