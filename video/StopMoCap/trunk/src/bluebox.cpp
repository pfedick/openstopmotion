/*
 * bluebox.cpp
 *
 *  Created on: 29.06.2012
 *      Author: patrick
 */

#include "bluebox.h"
#include <math.h>


typedef struct {
union  {
	struct { ppluint8 b,g,r,a; };
	ppluint32 c;
};
} PIXEL;

static inline int max(int a, int b)
{
   if (a>b) {return (a);}
   return (b);
}

static inline double colorclose(int Cb_p,int Cr_p,int Cb_key,int Cr_key,int tola,int tolb)
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


static inline int getYCb(int r, int g, int b)
{
	return (int) round(128 + -0.168736*r - 0.331264*g + 0.5*b);
}

static inline int getYCr(int r, int g, int b)
{
	return (int) round(128 + 0.5*r - 0.418688*g - 0.081312*b);
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

/*
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

			mask = 1-colorclose(cb, cr, cb_key, cr_key,tola,tolb);
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
*/


void BlueBox::process(ppl7::grafix::Image &img)
{
	double mask;
	int cb,cr;
	int cb_key=Key.getYCb();
	int cr_key=Key.getYCr();
	int r_key = Key.red();
	int g_key = Key.green();
	int b_key = Key.blue();

	PIXEL c,bg,t;

	ppluint32 *sadr=(ppluint32*)img.adr();
	ppluint32 spitch=img.pitch()/4;

	ppluint32 *bgadr=(ppluint32*)bgImage.adr();
	ppluint32 bgpitch=bgImage.pitch()/4;


	for (int y=0;y<img.height();y++) {
		for (int x=0;x<img.width();x++) {
			c.c=sadr[x];
			cb=getYCb(c.r,c.g,c.b);
			cr=getYCr(c.r,c.g,c.b);
			bg.c=bgadr[x];

			mask = 1-colorclose(cb, cr, cb_key, cr_key,tola,tolb);
			if (mask==0.0) {
				continue;
			} else if (mask==1.0) {
				sadr[x]=bg.c;
			} else {
				t.r=max(c.r-mask*r_key,0)+mask*bg.r;
				t.g=max(c.g-mask*g_key,0)+mask*bg.g;
				t.b=max(c.b-mask*b_key,0)+mask*bg.b;
				sadr[x]=bg.c;
			}
		}
		sadr+=spitch;
		bgadr+=bgpitch;
	}
}


