#!/usr/bin/python
import os
import web
import sys
import time
import signal

try:
    import RPi.GPIO as GPIO
    have_rpi_gpio = True
    from Adafruit_PWM_Servo_Driver import PWM
    pwm=PWM()
except ImportError:
    have_rpi_gpio = False
    
    
StatusRGBPins = [8,10,12]

StepPins = [16,18,22,24]
StepSeq = [[1,0,0,0],
       [1,1,0,0],
       [0,1,0,0],
       [0,1,1,0],
       [0,0,1,0],
       [0,0,1,1],
       [0,0,0,1],
       [1,0,0,1]]
StepCount = len(StepSeq)-1

SequencePosition=0

if have_rpi_gpio:
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(StatusRGBPins,GPIO.OUT)
    GPIO.output(StatusRGBPins, GPIO.HIGH)
    
    GPIO.setup(StepPins,GPIO.OUT)
    GPIO.output(StepPins, GPIO.LOW)
    
    
    
    
    
def setStatus(red, green, blue):
    global StatusRGBPins
    if not have_rpi_gpio:
        return
    GPIO.output(StatusRGBPins, GPIO.LOW)
    if red:
        GPIO.output(StatusRGBPins[0],GPIO.HIGH)
    if green:
        GPIO.output(StatusRGBPins[1],GPIO.HIGH)
    if blue:
        GPIO.output(StatusRGBPins[2],GPIO.HIGH)

def signalHandler(signum, frame):
    pass
    
    
def showform(steps):
    output = "<form method=GET url=/form>"
    output +="<input type=hidden name=step>"
    output +="<input name=steps size=10 value="+steps+">"
    output +="<input type=submit name=forward>"
    output +="<input type=submit name=backward>"
    output +="</form>\n"



class ShowHelp(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET():      # pylint: disable=C0103
        setStatus(0,0,1)
        output ="<html><b>Interface:</b><br>\n"
        output +="</html>\n"
        setStatus(0,1,0)
        return output
    
class Reset(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET():      # pylint: disable=C0103
        setStatus(0,0,1)
        SequencePosition=0
        setStatus(0,1,0)
        return "Step Sequence resetted"
    
class HandleFormular(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET():      # pylint: disable=C0103
        return ""
    
class SetPwm(object):
    def __init__(self):
        pass
    @staticmethod
    def GET(channel, value):      # pylint: disable=C0103
        global pwm
        setStatus(0,0,1)
        if have_rpi_gpio:
            if (int(value)==0):
                pwm.setPWM(int(channel),4096,0)
            pwm.setPWM(int(channel),0,int(value))
        
        setStatus(0,1,0)
        return "OK"
    
class StepMove(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET(direction, steps):      # pylint: disable=C0103
        global SequencePosition
        setStatus(0,0,1)
        if direction == "forward":
            for count in range (0,int(steps)):
                #print StepSeq[SequencePosition]
                if have_rpi_gpio:
                    for pin in range (0,4):
                        GPIO.output(StepPins[pin],StepSeq[SequencePosition][pin])
                SequencePosition = SequencePosition + 1
                if SequencePosition >= StepCount:
                    SequencePosition=0
                time.sleep(0.010)
   
            
        elif direction == "backward":
            SequencePosition -= 1
            if SequencePosition<0:
                SequencePosition=StepCount

            for count in range (0,int(steps)):
                SequencePosition -= 1
                if SequencePosition<0:
                    SequencePosition=StepCount
                #print StepSeq[SequencePosition]
                if have_rpi_gpio:
                    for pin in range (0,4):
                        GPIO.output(StepPins[pin],StepSeq[SequencePosition][pin])
                time.sleep(0.010)
        if have_rpi_gpio:
            GPIO.output(StepPins, GPIO.LOW)
        setStatus(0,1,0)
        return "OK. Steps "+str(direction)+": "+str(steps)



if __name__ == "__main__":
    urls = (
            '/', 'ShowHelp',
            '/step/(.*)/(.*)', 'StepMove',
            '/reset', 'Reset',
            '/form', 'HandleFormular',
            '/pwm/(.*)/(.*)', 'SetPwm'
            )
    web.config.debug = False
    setStatus(0,1,0)
    if have_rpi_gpio:
        pwm.setPWMFreq(1000)
        for channel in range (0,15):
            pwm.setPWM(channel,0,0)
    app=web.application(urls, globals())
    app.run()
    setStatus(0,0,0)
    if have_rpi_gpio:
        GPIO.cleanup()
            