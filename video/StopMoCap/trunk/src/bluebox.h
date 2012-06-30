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
		ppl7::grafix::Color Replace;
		ppl7::grafix::Image bgImage;
		ppl7::grafix::Image fgImage;

		int tola;
		int tolb;
		int spill;

		int mode;

		//double colorclose(int Cb_p,int Cr_p,int Cb_key,int Cr_key);

	public:
		BlueBox();
		void setColorKey(const ppl7::grafix::Color &key);
		void setBackground(const ppl7::grafix::Drawable &img);
		void loadBackground(const ppl7::String &file);
		void setReplaceColor(const ppl7::grafix::Color &color);
		void setReplaceMode(int mode);
		void setForeground(const ppl7::grafix::Drawable &img);
		void setNearTolerance(int value);
		void setFarTolerance(int value);
		void setSpillRemoval(int value);

		int nearTolerance() const;
		int farTolerance() const;
		int spillRemoval() const;
		ppl7::grafix::Color colorKey() const;
		const ppl7::grafix::Image & getBGImage() const;

		void process(ppl7::grafix::Image &img);
};



#endif /* BLUEBOX_H_ */
