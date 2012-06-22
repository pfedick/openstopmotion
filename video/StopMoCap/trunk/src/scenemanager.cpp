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
	Frame &frame=frames[nr];
	frame.Filename=Filename;
	if (nr>highest) highest=nr;
}

void SceneManager::setImage(size_t nr, const ppl7::grafix::Image &img)
{
	std::map<size_t,Frame>::iterator it;
	Frame &frame=frames[nr];
	ppl7::grafix::ImageFilter_JPEG jpeg;
	ppl7::AssocArray param;
	param.set("quality","85");
	frame.file.rewind();
	jpeg.save(img,frame.file,param);
	frame.file.truncate(frame.file.size());
}

const ppl7::String &SceneManager::getFilename(size_t nr) const
{

}

void SceneManager::getImage(size_t nr, ppl7::grafix::Image &img) const
{

}

size_t SceneManager::getHighestFrameNum() const
{

}
