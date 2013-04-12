#define USESERIAL 1

int digit[3] = { 46,48,50 };
const int numPwmLeds=12;
const int pwmled[numPwmLeds] = { 2,4,6,8,10,12,3,5,7,9,11,13 };
const int reset = 52;

int previousValue=0;
int currentValue=0;
const int delayTime=0;

int frame=0;


void resetToNull()
{
  digitalWrite(reset, LOW);
  delay(delayTime); 
  digitalWrite(reset, HIGH);
  delay(delayTime); 
  digitalWrite(reset, LOW);
}

void incrementDigit(int n)
{
  digitalWrite(digit[n], HIGH);
  delay(delayTime); 
  digitalWrite(digit[n], LOW);
  delay(delayTime);
}

void setDigit(int n, int value)
{
  for (int i=0;i<value;i++) {
      incrementDigit(n);  
  }
}

void setValue(int value)
{
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
#ifdef USESERIAL
      if (Serial.available()>0) return;
#endif
    }
  
    for (z=0;z<255;z++) {
      setValue(z);
      for (i=0;i<numPwmLeds;i++) {
        analogWrite(pwmled[i], z);
      }
#ifdef USESERIAL
      if (Serial.available()>0) return;
#endif
      delay(5);
    }
    for (z=255;z>=0;z--) {
      setValue(z);
      for (i=0;i<numPwmLeds;i++) {
        analogWrite(pwmled[i], z);
      }
#ifdef USESERIAL
      if (Serial.available()>0) return;
#endif
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
#ifdef USESERIAL
  int num,value;
  num=waitForByte();
  value=waitForByte();
  if (num>=0 && num<numPwmLeds) analogWrite(pwmled[num], value);
#endif
}

void functionSetCounter()
{
#ifdef USESERIAL
  int value;
  int bytes;
  bytes=Serial.readBytes((char*)&value,2);
  if (bytes!=2) value=999; 
  if (value>=0 && value<1000) setValue(value);
  else setValue(999);
#endif
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


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(digit[0], OUTPUT);     
  pinMode(digit[1], OUTPUT);     
  pinMode(digit[2], OUTPUT);     
  pinMode(reset, OUTPUT);
  digitalWrite(digit[0], LOW);  
  digitalWrite(digit[1], LOW);  
  digitalWrite(digit[2], LOW);  
  resetToNull();
  
  for (int i=0;i<numPwmLeds;i++) {
    pinMode(pwmled[i], OUTPUT);
    analogWrite(pwmled[i], 0);
  }
  //analogWrite(pwmled[0], 255);
  
#ifdef USESERIAL
   Serial.begin(57600);
  #endif
  setValue(123);
  demoShow1();
}

void loop()
{
#ifdef USESERIAL
  //demoShow1();
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
#else
   demoShow1();
#endif
}
