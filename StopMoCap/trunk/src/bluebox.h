/*
 * bluebox.h
 *
 *  Created on: 29.06.2012
 *      Author: patrick
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
