/*
 * scenemanager.h
 *
 *  Created on: 15.06.2012
 *      Author: patrickf
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#define WITH_QT
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <vector>

class Frame
{
	public:
		ppl7::String Filename;
		ppl7::ByteArray img;
};

class SceneManager
{
	private:
		ppl7::Mutex mutex;
		ppl7::String path;
		std::vector<Frame> frames;
		size_t highest;

	public:
		SceneManager();
		~SceneManager();

		void clear();
		void loadScene(const ppl7::String &path);
		void setFilename(size_t nr, const ppl7::String Filename);
		void setImage(size_t nr, const ppl7::grafix::Image &img);
		void deleteFrame(size_t nr);
		const ppl7::String &getFilename(size_t nr) const;
		void getImage(size_t nr, ppl7::grafix::Image &img) const;
		size_t getHighestFrameNum() const;

};


#endif /* SCENEMANAGER_H_ */
