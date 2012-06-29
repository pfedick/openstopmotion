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
		ppl7::grafix::Image bgImage;
		ppl7::grafix::Image fgImage;

		int tola;
		int tolb;

		double colorclose(int Cb_p,int Cr_p,int Cb_key,int Cr_key);

	public:
		BlueBox();
		void setColorKey(const ppl7::grafix::Color &key);
		void setBackground(const ppl7::grafix::Drawable &img);
		void loadBackground(const ppl7::String &file);
		void setForeground(const ppl7::grafix::Drawable &img);
		void setNearTolerance(int value);
		void setFarTolerance(int value);

		int nearTolerance() const;
		int farTolerance() const;
		ppl7::grafix::Color colorKey() const;

		void process(ppl7::grafix::Image &img);
		void chromaInfo(ppl7::String &out, const ppl7::grafix::Color &c);
};



#endif /* BLUEBOX_H_ */
