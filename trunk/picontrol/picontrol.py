#!/usr/bin/python
import os
import web
import sys
import time

try:
    import RPi.GPIO as GPIO
    have_rpi_gpio = True
except ImportError:
    have_rpi_gpio = False
    
    
  

StepPins = [10,12,16,18]
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
    GPIO.setup(StepPins,GPIO.OUT)
    GPIO.output(StepPins, GPIO.LOW)
    
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
        output ="<html><b>Interface:</b><br>\n"
        output +="</html>\n"
        return output
    
class Reset(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET():      # pylint: disable=C0103
        SequencePosition=0
        return ""
    
class HandleFormular(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET():      # pylint: disable=C0103
        return ""
    
    
class StepMove(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET(direction, steps):      # pylint: disable=C0103
        global SequencePosition
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
        return "OK. Steps "+str(direction)+": "+str(steps)
    


if __name__ == "__main__":
    urls = (
            '/', 'ShowHelp',
            '/step/(.*)/(.*)', 'StepMove',
            '/reset', 'Reset'
            '/form', 'HandleFormular'
            )
    web.config.debug = False
    app=web.application(urls, globals())
    app.run()
            