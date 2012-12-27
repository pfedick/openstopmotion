/*
 * FrameBuffer.h
 *
 *  Created on: 08.09.2012
 *      Author: patrick
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <map>
#include <ppl7.h>
#include <ppl7-grafix.h>

class FrameBuffer : private ppl7::Thread
{
	private:
		std::map<int,ppl7::grafix::Image> Frames;
		ppl7::Mutex mutex;
		ppl7::String Path;

		void run();
		void loadNow(size_t nr);

	public:
		FrameBuffer();
		virtual ~FrameBuffer();

		void pushBack(const ppl7::grafix::Drawable &img);
		void setImage(size_t nr, const ppl7::grafix::Drawable &img);
		const ppl7::grafix::Drawable &getImage(size_t nr);
		void copyImage(size_t nr, ppl7::grafix::Image &img);
		void popBack();
		void loadAsync(const ppl7::String &Dir);
		void clear();
		size_t size();

};

#endif /* FRAMEBUFFER_H_ */
