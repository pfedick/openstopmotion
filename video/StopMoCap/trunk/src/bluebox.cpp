/*
 * bluebox.cpp
 *
 *  Created on: 29.06.2012
 *      Author: patrick
 */

#include "bluebox.h"
#include <math.h>

int max(int a, int b)
{
   if (a>b) {return (a);}
   return (b);
}

double BlueBox::colorclose(int Cb_p,int Cr_p,int Cb_key,int Cr_key)
{
   /*decides if a color is close to the specified hue*/
   double temp = sqrt((Cb_key-Cb_p)*(Cb_key-Cb_p)+(Cr_key-Cr_p)*(Cr_key-Cr_p));
   // SSE: sqrtss für float, SSE2: sqrtsd für double
   // Man könnte Cb und Cr in ein MME-Register packen, die Subtraktionen und Multiplikationen
   // parallel berechnen, das Ergebnis addieren und dann die Wurzel ziehen
   if (temp < tola) {return (0.0);}
   if (temp < tolb) {return ((temp-tola)/(tolb-tola));}
   return (1.0);
}

BlueBox::BlueBox()
{
	Key.set(0,0,255);
	tola=50;
	tolb=70;
}

void BlueBox::setColorKey(const ppl7::grafix::Color &key)
{
	Key=key;
}


void BlueBox::loadBackground(const ppl7::String &file)
{
	bgImage.load(file);
}

void BlueBox::setBackground(const ppl7::grafix::Drawable &img)
{
	bgImage.copy(img);
}

void BlueBox::setForeground(const ppl7::grafix::Drawable &img)
{
	fgImage.copy(img);
}

void BlueBox::setNearTolerance(int value)
{
	tola=value;
}

void BlueBox::setFarTolerance(int value)
{
	tolb=value;
}

int BlueBox::nearTolerance() const
{
	return tola;
}

int BlueBox::farTolerance() const
{
	return tolb;
}

ppl7::grafix::Color BlueBox::colorKey() const
{
	return Key;
}


void BlueBox::process(ppl7::grafix::Image &img)
{
	double mask;
	int cb,cr;
	int cb_key=Key.getYCb();
	int cr_key=Key.getYCr();
	int r_key = Key.red();
	int g_key = Key.green();
	int b_key = Key.blue();

	ppl7::grafix::Color c,t,bg;

	for (int y=0;y<img.height();y++) {
		for (int x=0;x<img.width();x++) {
			c=img.getPixel(x,y);
			cb=c.getYCb();
			cr=c.getYCr();
			bg=bgImage.getPixel(x,y);

			mask = 1-colorclose(cb, cr, cb_key, cr_key);
			if (mask==0.0) {
				continue;
			} else if (mask==1.0) {
				img.putPixel(x,y,bg);
			} else {
				t.set(max(c.red()-mask*r_key,0)+mask*bg.red(),
						max(c.green()-mask*g_key,0)+mask*bg.green(),
						max(c.blue()-mask*b_key,0)+mask*bg.blue()
				);
				img.putPixel(x,y,t);
			}
		}
	}
}

void BlueBox::chromaInfo(ppl7::String &out, const ppl7::grafix::Color &c)
{
	double mask;
	int cb,cr;
	int cb_key=Key.getYCb();
	int cr_key=Key.getYCr();
	int r_key = Key.red();
	int g_key = Key.green();
	int b_key = Key.blue();
	cb=c.getYCb();
	cr=c.getYCr();
	mask = 1-colorclose(cb, cr, cb_key, cr_key);
	out.appendf(", mask=%0.3f",mask);

	/*
	t.set(max(c.red()-mask*r_key,0)+mask*bg.red(),
						max(c.green()-mask*g_key,0)+mask*bg.green(),
						max(c.blue()-mask*b_key,0)+mask*bg.blue()
						);
						*/
}

