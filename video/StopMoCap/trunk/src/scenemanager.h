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
#include <map>

class SceneManager
{
	private:
		class Frame
		{
			public:
				ppl7::String Filename;
				ppl7::MemFile file;
		};

		class LoaderThread : public ppl7::Thread
		{
			public:
				SceneManager *manager;

				void run();

		};


		ppl7::Mutex mutex;
		LoaderThread lthread;
		ppl7::String path;
		std::map<size_t,Frame> frames;
		size_t highest;

	public:
		SceneManager();
		~SceneManager();

		void clear();
		void loadScene(const ppl7::String &path);
		void setFilename(size_t nr, const ppl7::String Filename);
		void setImage(size_t nr, const ppl7::grafix::Image &img);
		void deleteFrame(size_t nr);
		ppl7::String getFilename(size_t nr);
		void getImage(size_t nr, ppl7::grafix::Image &img);
		size_t getHighestFrameNum();

};


#endif /* SCENEMANAGER_H_ */
