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
