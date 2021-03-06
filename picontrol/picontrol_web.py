#!/usr/bin/python
import os
import web
import sys
import time
import signal
from datetime import datetime

try:
    import RPi.GPIO as GPIO
    have_rpi_gpio = True
    from Adafruit_PWM_Servo_Driver import PWM
    pwm=PWM()
except ImportError:
    have_rpi_gpio = False
    
    
StatusRGBPins = [8,10,12]

Motor1_dir = 11
Motor1_step = 13
Motor1_sleep = 15
Motor1_sensor = 16
Motor1_stop_detected=""


if have_rpi_gpio:
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(StatusRGBPins,GPIO.OUT)
    GPIO.output(StatusRGBPins, GPIO.HIGH)
    

    GPIO.setup(Motor1_dir,GPIO.OUT)    
    GPIO.setup(Motor1_step,GPIO.OUT)    
    GPIO.setup(Motor1_sleep,GPIO.OUT)    
    GPIO.setup(Motor1_sensor,GPIO.IN, GPIO.PUD_UP)    
    GPIO.output(Motor1_dir, GPIO.LOW)
    GPIO.output(Motor1_step, GPIO.LOW)
    GPIO.output(Motor1_sleep, GPIO.LOW)
    
    
    
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

def signal_term_handler(signum, frame):
    setStatus(0,0,0)
    if have_rpi_gpio:
        GPIO.cleanup()
    sys.exit(0)
    
def showform(steps):
    output = "<form method=GET url=/form>"
    output +="<input type=hidden name=step>"
    output +="<input name=steps size=10 value="+steps+">"
    output +="<input type=submit name=forward>"
    output +="<input type=submit name=backward>"
    output +="</form>\n"

def exact_sleep(seconds):
    start = datetime.now()
    startseconds=start.second + start.microsecond/1000000
    endsconds=startseconds+seconds
    print time.time()
    print time.time()
    print time.time()
    print time.time()
    print time.time()
    print time.time()
    time.sleep(seconds)
    return
    while True:
        now = datetime.now()
        nowseconds=now.second + now.microsecond/1000000
	print nowseconds, endsconds
        if nowseconds>=endsconds:
            print "debug"
            return

def check_sleep(amount):
    start = datetime.now()
    time.sleep(amount)
    end = datetime.now()
    delta = end-start
    return delta.seconds + delta.microseconds/1000000.


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
    
class Enumerate(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET():      # pylint: disable=C0103
        web.header('Content-Type', 'text/xml')
        output="<motioncontrol><enumerate>\n";
        for counter in range (0,16):
            output+="   <device><uri>/lights/pwm/"+str(counter)+"/</uri>" + \
                "<id>light"+str(counter)+"</id>" + \
                "<name>Light "+str(counter+1)+"</name><type>led</type><pwm><min>0</min><max>4095</max></pwm></device>\n"
        output+="<device><uri>/camera/move/</uri><type>stepmotor</type><name>Camera Move</name><id>motor0</id>" + \
            "</device>\n";    
        output+="</enumerate></motioncontrol>\n";
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
    
class StepMoveBipolar(object):
    def __init__(self):
        pass
    
    @staticmethod
    def GET(direction, steps):      # pylint: disable=C0103
        global Motor1_stop_detected
        setStatus(0,0,1)
        if direction == "right":
            GPIO.output(Motor1_dir, GPIO.LOW)
        else:
            GPIO.output(Motor1_dir, GPIO.HIGH)
        GPIO.output(Motor1_sleep, GPIO.HIGH)
        time.sleep(0.002)
        for count in range (0,int(steps)):
            if GPIO.input(Motor1_sensor) == False:
                if Motor1_stop_detected == "":
                    Motor1_stop_detected = direction
            else:
                Motor1_stop_detected = ""        
            if Motor1_stop_detected == direction:
                setStatus(0,1,0)
                return "ERROR: Stop signal received"
            GPIO.output(Motor1_step, GPIO.HIGH)
            #time.sleep(0.001)
            GPIO.output(Motor1_step, GPIO.LOW)
            time.sleep(0.001)
        #GPIO.output(Motor1_sleep, GPIO.LOW)
        time.sleep(0.002)
        setStatus(0,1,0)
        return "OK. Steps "+str(direction)+": "+str(steps)

def statustest():
    setStatus(1,1,1)
    time.sleep(0.5)
    setStatus(1,0,0)
    time.sleep(0.5)
    setStatus(0,1,0)
    time.sleep(0.5)
    setStatus(0,0,1)
    time.sleep(0.5)
    setStatus(1,1,0)
    time.sleep(0.5)
    setStatus(0,1,1)
    time.sleep(0.5)
    setStatus(1,0,1)
    time.sleep(0.5)
    setStatus(0,0,0)

if __name__ == "__main__":
    signal.signal(signal.SIGTERM, signal_term_handler)
    urls = (
            '/', 'ShowHelp',
            '/camera/move/(.*)/(.*)', 'StepMoveBipolar',
            '/reset', 'Reset',
            '/form', 'HandleFormular',
            '/lights/pwm/(.*)/(.*)', 'SetPwm',
            '/enumerate','Enumerate'
            )
    web.config.debug = False
    statustest()

    if have_rpi_gpio:
        pwm.setPWMFreq(1000)
        for channel in range (0,15):
            pwm.setPWM(channel,0,0)
    app=web.application(urls, globals())
    setStatus(0,1,0)
    app.run()
    setStatus(0,0,0)
    if have_rpi_gpio:
        GPIO.cleanup()


