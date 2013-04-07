/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick
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

#ifndef BLUEBOX_H_
#define BLUEBOX_H_

#include <ppl7.h>
#include <ppl7-grafix.h>

class BlueBox
{
	private:
		ppl7::grafix::Color Key;
		ppl7::grafix::Color KeyFG;
		ppl7::grafix::Color Replace;
		ppl7::grafix::Image bgImage;
		ppl7::grafix::Image fgImage;
		ppl7::grafix::Image repImage;

		ppl7::grafix::Image fgImageAlpha;



		int tola;
		int tolb;
		int spill;

		int tolaFG;
		int tolbFG;
		int spillFG;

		int mode;

		bool bgChromaEnabled;
		bool fgChromaEnabled;

		//double colorclose(int Cb_p,int Cr_p,int Cb_key,int Cr_key);

		void UpdateForeground();
		void UpdateReplaceColor();

	public:
		BlueBox();
		void setColorKey(const ppl7::grafix::Color &key);
		void setColorKeyFG(const ppl7::grafix::Color &key);
		void setBackground(const ppl7::grafix::Drawable &img);
		void loadBackground(const ppl7::String &file);
		void loadForeground(const ppl7::String &file);
		void setReplaceColor(const ppl7::grafix::Color &color);
		void setReplaceMode(int mode);
		void setForeground(const ppl7::grafix::Drawable &img);
		void setNearTolerance(int value);
		void setFarTolerance(int value);
		void setSpillRemoval(int value);

		void setNearToleranceFG(int value);
		void setFarToleranceFG(int value);
		void setSpillRemovalFG(int value);


		void setBGChromaEnabled(bool flag);
		void setFGChromaEnabled(bool flag);

		int nearTolerance() const;
		int farTolerance() const;
		int spillRemoval() const;
		int nearToleranceFG() const;
		int farToleranceFG() const;
		int spillRemovalFG() const;


		ppl7::grafix::Color colorKey() const;
		ppl7::grafix::Color colorKeyFG() const;
		ppl7::grafix::Color replaceColor() const;
		const ppl7::grafix::Image & getBGImage() const;
		const ppl7::grafix::Image & getFGImage() const;

		void process(ppl7::grafix::Image &img);
};



#endif /* BLUEBOX_H_ */
