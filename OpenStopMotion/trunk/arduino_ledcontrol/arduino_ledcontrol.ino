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

///////////////////////////////////////////////////////////////////////////
/*
 * You can implement a numeric display with 3 numbers. The display has to
 * be controlled with 4 pins:
 *    - one pin to reset all digets to zero
 *    - one pin to increment the first digit
 *    - one pin to increment the second digit
 *    - one pin to increment the third digit
 * 
 * If one digit reaches the value 9 and is incremented, it must get zero,
 * but it must not increment the next digit (the sketch takes care of that).
 * I used 3 IC 4026 decade counter for this, each can drive one 7 segment led
 * digit.
 */
const int digit[3] = { 46,48,50 };
const int digit_reset = 52;

/*
 * You can implement up to 12 PWM channels. You must define how many
 * channels you have, and to which pins they are connected
 */
const int numPwmLeds=12;
const int pwmled[numPwmLeds] = { 2,4,6,8,10,12,3,5,7,9,11,13 };




/*
 * You don't have to change anything beyond this line!
 */
int previousValue=0;
int currentValue=0;
const int delayTime=0;
int frame=0;


void resetToNull()
{
	if(digit_reset!=0) {
		digitalWrite(digit_reset, LOW);
		delay(delayTime); 
		digitalWrite(digit_reset, HIGH);
		delay(delayTime); 
		digitalWrite(digit_reset, LOW);
	}
}

void incrementDigit(int n)
{
	if(digit_reset!=0) {
		digitalWrite(digit[n], HIGH);
		delay(delayTime); 
		digitalWrite(digit[n], LOW);
		delay(delayTime);
	}
}

void setDigit(int n, int value)
{
	if(digit_reset!=0) {
		for (int i=0;i<value;i++) {
			incrementDigit(n);  
		}
	}
}

void setValue(int value)
{
	if(digit_reset!=0) {
		if (value == currentValue) return;
		if (value>999 || value==0) {
			currentValue=0;
			resetToNull();
			return;
		}
		if (value == currentValue+1) {
			incrementDigit(0);
			if ((value%10)==0) incrementDigit(1);
			if ((value%100)==0) incrementDigit(2);
			currentValue=value;
			return;
		}
		currentValue=value;
		resetToNull();
		int d;
		d=value%10;
		setDigit(0,d);
		value=value/10;
		d=value%10;
		setDigit(1,d);
		value=value/10;
		d=value%10;
		setDigit(2,d);
	}
}

void testValue(int value)
{
	setValue(value);
	delay(1000);
}

void demoShow1()
{
	int i,z;
	functionSetAllLightsOff();

	while (1) {
		for (i=0;i<numPwmLeds;i++) {
			analogWrite(pwmled[i], 255);
			testValue(i+1);
			analogWrite(pwmled[i], 0);
			if (Serial.available()>0) return;
		}

		for (z=0;z<255;z++) {
			setValue(z);
			for (i=0;i<numPwmLeds;i++) {
				analogWrite(pwmled[i], z);
			}
			if (Serial.available()>0) return;
			delay(5);
		}
		for (z=255;z>=0;z--) {
			setValue(z);
			for (i=0;i<numPwmLeds;i++) {
				analogWrite(pwmled[i], z);
			}
			if (Serial.available()>0) return;
			delay(5);
		} 
	}
}

int waitForByte()
{
	int byte;
	while (1) {
		byte=Serial.read();
		if (byte!=-1) break;
	}
	return byte;
}




void functionSetLightValue()
{
	int num,value;
	num=waitForByte();
	value=waitForByte();
	if (num>=0 && num<numPwmLeds) analogWrite(pwmled[num], value);
}

void functionSetCounter()
{
	int value;
	int bytes;
	bytes=Serial.readBytes((char*)&value,2);
	if (bytes!=2) value=999; 
	if (value>=0 && value<1000) setValue(value);
	else setValue(999);
}

void functionSetAllLightsOn()
{
	int i;
	for (i=0;i<numPwmLeds;i++) {
		analogWrite(pwmled[i], 255);
	}
}

void functionSetAllLightsOff()
{
	int i;
	for (i=0;i<numPwmLeds;i++) {
		analogWrite(pwmled[i], 0);
	}
}


void setup() {
	if (digit_reset!=0) {
		pinMode(digit[0], OUTPUT);     
		pinMode(digit[1], OUTPUT);     
		pinMode(digit[2], OUTPUT);     
		pinMode(digit_reset, OUTPUT);
		digitalWrite(digit[0], LOW);  
		digitalWrite(digit[1], LOW);  
		digitalWrite(digit[2], LOW);  
		resetToNull();
	}
	for (int i=0;i<numPwmLeds;i++) {
		pinMode(pwmled[i], OUTPUT);
		analogWrite(pwmled[i], 0);
	}
	Serial.begin(57600);
	setValue(123);
	demoShow1();
}

void loop()
{
	int incomingByte = 0;
	while(Serial) {
		while (Serial.available()>0) {
			incomingByte = waitForByte();
			if (incomingByte==1) functionSetLightValue();
			else if (incomingByte==2) functionSetCounter();
			else if (incomingByte==3) functionSetAllLightsOn();
			else if (incomingByte==4) functionSetAllLightsOff();
			else if (incomingByte==5) demoShow1();
		}
	}
}
