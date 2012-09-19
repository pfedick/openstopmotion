/*
 * bluebox.cpp
 *
 *  Created on: 29.06.2012
 *      Author: patrick
 */

#include "bluebox.h"
#include <math.h>

BlueBox::BlueBox()
{
	Key.set(0,0,255);
	KeyFG.set(0,0,255);
	tola=50;
	tolb=70;
	spill=0;
	mode=0;
	fgChromaEnabled=false;
	bgChromaEnabled=false;
	tolaFG=50;
	tolbFG=70;
	spillFG=0;

}

void BlueBox::setBGChromaEnabled(bool flag)
{
	bgChromaEnabled=flag;
}

void BlueBox::setFGChromaEnabled(bool flag)
{
	fgChromaEnabled=flag;
	UpdateForeground();
}

void BlueBox::setColorKey(const ppl7::grafix::Color &key)
{
	Key=key;
}

void BlueBox::setColorKeyFG(const ppl7::grafix::Color &key)
{
	KeyFG=key;
	UpdateForeground();
}

void BlueBox::setReplaceColor(const ppl7::grafix::Color &color)
{
	Replace=color;
	UpdateReplaceColor();
}

void BlueBox::setReplaceMode(int mode)
{
	if (mode==1) {
		this->mode=1;
		UpdateReplaceColor();
	} else if (mode==2) {
		this->mode=2;
		UpdateReplaceColor();
	}
	else this->mode=0;
}

void BlueBox::loadBackground(const ppl7::String &file)
{
	bgImage.load(file);
}

void BlueBox::loadForeground(const ppl7::String &file)
{
	fgImage.load(file);
	UpdateForeground();
}

void BlueBox::setBackground(const ppl7::grafix::Drawable &img)
{
	bgImage.copy(img);
}

void BlueBox::setForeground(const ppl7::grafix::Drawable &img)
{
	fgImage.copy(img);
	UpdateForeground();
}

void BlueBox::setSpillRemoval(int value)
{
	spill=value;
}

void BlueBox::setNearTolerance(int value)
{
	tola=value;
}

void BlueBox::setFarTolerance(int value)
{
	tolb=value;
}

void BlueBox::setSpillRemovalFG(int value)
{
	spillFG=value;
	UpdateForeground();
}

void BlueBox::setNearToleranceFG(int value)
{
	tolaFG=value;
	UpdateForeground();
}

void BlueBox::setFarToleranceFG(int value)
{
	tolbFG=value;
	UpdateForeground();
}


int BlueBox::nearTolerance() const
{
	return tola;
}

int BlueBox::farTolerance() const
{
	return tolb;
}

int BlueBox::spillRemoval() const
{
	return spill;
}

int BlueBox::nearToleranceFG() const
{
	return tolaFG;
}

int BlueBox::farToleranceFG() const
{
	return tolbFG;
}

int BlueBox::spillRemovalFG() const
{
	return spillFG;
}

const ppl7::grafix::Image & BlueBox::getBGImage() const
{
	return bgImage;
}

const ppl7::grafix::Image & BlueBox::getFGImage() const
{
	return fgImage;
}

ppl7::grafix::Color BlueBox::colorKey() const
{
	return Key;
}

ppl7::grafix::Color BlueBox::colorKeyFG() const
{
	return KeyFG;
}

ppl7::grafix::Color BlueBox::replaceColor() const
{
	return Replace;
}


void BlueBox::UpdateForeground()
{
	if (fgChromaEnabled==false) return;
	if (fgImage.isEmpty()) return;
	fgImageAlpha.create(fgImage.width(),fgImage.height(),fgImage.rgbformat());
	fgImageAlpha.bltChromaKey(fgImage,KeyFG,tolaFG,tolbFG);
}


void BlueBox::UpdateReplaceColor()
{
	if (repImage.isEmpty()) return;
	if (mode==2) {
		ppl7::grafix::Color c[2];
		c[0].set(255,255,255,0);
		c[1].set(230,230,230,0);
		int n;
		int z=0;
		for (int y=0;y<repImage.height();y+=8) {
			n=z&1;
			z++;
			for (int x=0;x<repImage.width();x+=8) {
				repImage.fillRect(x,y,x+8,y+8,c[n]);
				n=!n;
			}

		}
	}
	else repImage.cls(Replace);
}


void BlueBox::process(ppl7::grafix::Image &img)
{
	if (bgChromaEnabled) {

		if (mode==0) {
			img.bltBackgroundOnChromaKey(bgImage,Key,tola,tolb);
		} else {
			if (repImage.isEmpty()==true || repImage.width()!=img.width()
					|| repImage.height()!=img.height()) {
				repImage.create(img.width(),img.height(),ppl7::grafix::RGBFormat::A8R8G8B8);
				UpdateReplaceColor();
			}
			img.bltBackgroundOnChromaKey(repImage,Key,tola,tolb);
		}


	}
	if (fgChromaEnabled==true && fgImageAlpha.isEmpty()==false) {
		img.bltAlpha(fgImageAlpha);
	}
}


