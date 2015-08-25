#!/usr/bin/python
import os
import sys
import time
import signal
from datetime import datetime
import web
import socket
import threading
import SocketServer
import select
from argparse import ArgumentParser

from config import Config

try:
    import RPi.GPIO as GPIO
    have_rpi_gpio = True
    from Adafruit_PWM_Servo_Driver import PWM
except ImportError:
    have_rpi_gpio = False
    

class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):

    def handle(self):
        osm.add_connection()
        try:
            while True:
                data = self.request.recv(1024)
                if data[0]=='q':
                    break
                else:
                    self.request.sendall(osm.handle_request(data))
        except:
            pass
        osm.remove_connection()
        print "Connection terminated"

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass


class OpenStopMotionPiControl(object):
    def __init__(self):
        self.config=Config()
        self.args = None
        self.pwm = None
        self.server = None
        self.stopflag = False
        self.connection_count = 0
        
    def parse_args(self, args=None):
        self.parser = ArgumentParser (description = 'TCP-Server '
                                      'for controling lights and camera motors with OpenStopMotion')
        self.parser.add_argument('-c', type=str, dest='configfile', required=False,
                                 help='configuration file',
                                 default='picontrol.conf')
        if args:
            self.args = self.parser.parse_args(args)
        else:
            self.args = self.parser.parse_args()
        
    def load_config(self):
        """!Load the configuration file"""
        self.config.load(self.args.configfile)
        
    def init_gpio(self):
        """!Initialize the GPIO pins"""
        if have_rpi_gpio:
            GPIO.setmode(GPIO.BOARD)
            if (self.config.StatusRGBPins.red):
                GPIO.setup(self.config.StatusRGBPins.red,GPIO.OUT, initial=GPIO.LOW)
                GPIO.setup(self.config.StatusRGBPins.green,GPIO.OUT, initial=GPIO.LOW)
                GPIO.setup(self.config.StatusRGBPins.blue,GPIO.OUT, initial=GPIO.LOW)
                self.set_rgb_status(1,1,1)
            if (self.config.pwm_i2c_address>0):
                self.pwm = PWM(self.config.pwm_i2c_address)
                self.pwm.setPWMFreq(self.config.pwm_frequency)
            if (self.config.Motor1.dir>0):
                GPIO.setup(self.config.Motor1.dir,GPIO.OUT, initial=GPIO.LOW)    
                GPIO.setup(self.config.Motor1.step,GPIO.OUT, initial=GPIO.LOW)    
                GPIO.setup(self.config.Motor1.sleep,GPIO.OUT, initial=GPIO.LOW)    
                if (self.config.Motor1.stopsensor>0):
                    GPIO.setup(self.config.Motor1.stopsensor,GPIO.IN, GPIO.PUD_UP)    
            if (self.config.Motor2.dir>0):
                GPIO.setup(self.config.Motor2.dir,GPIO.OUT, initial=GPIO.LOW)    
                GPIO.setup(self.config.Motor2.step,GPIO.OUT, initial=GPIO.LOW)    
                GPIO.setup(self.config.Motor2.sleep,GPIO.OUT, initial=GPIO.LOW)
                if (self.config.Motor2.stopsensor>0):
                    GPIO.setup(self.config.Motor2.stopsensor,GPIO.IN, GPIO.PUD_UP)    
        
    def set_rgb_status(self, red, green, blue):
        """"""
        if have_rpi_gpio==True and self.config.StatusRGBPins.red>0:
            GPIO.output(self.config.StatusRGBPins.red,red)
            GPIO.output(self.config.StatusRGBPins.green,green)
            GPIO.output(self.config.StatusRGBPins.blue,blue)
        
    def start_server(self):
        """!Start TCP-Server"""
        self.server = ThreadedTCPServer((self.config.hostname, self.config.port),
                                        ThreadedTCPRequestHandler)
        self.server.allow_reuse_address=True
        print "Starting TCP-Server"
        self.stopflag=False
        srvfd = self.server.fileno()
        try:
            self.set_rgb_status(0,1,0)
            while self.stopflag==False:
                if self.connection_count>0:
                    self.set_rgb_status(0,1,1)
                else:
                    self.set_rgb_status(0,1,0)
                ready = select.select([srvfd], [], [], 0.5)
                if srvfd in ready[0]:
                    self.server.handle_request()
        except:
            self.set_rgb_status(1,0,0)
            pass
        print ("TCP-Server stopped")
        
    def stop_server(self):
        self.stopflag=True
        print "Shutdown triggered"

    def add_connection(self):
        self.connection_count = self.connection_count + 1

    def remove_connection(self):
        if self.connection_count>0:
            self.connection_count = self.connection_count - 1
         
    def handle_pwm(self, channel, value):
        """!Handles a PWM request"""
        if channel>15 or value>4095:
            self.set_rgb_status(1,0,0)
            return "Error: Invalid PWM parameters [channel="+str(channel)+", value="+str(value)+"]\n"
        if not self.pwm:
            return "Error: PWM not configured\n"
        self.set_rgb_status(0,0,1)
        if have_rpi_gpio:
            if value==0:
                self.pwm.setPWM(channel,4096,0)
            else:
                self.pwm.setPWM(channel,0,value)
        return "Ok\n"  

    def handle_request(self,data):
        cmd=data.split(";")
        if cmd[0]=='pwm':
            if len(cmd)!=3:
                return "Error: Invalid command\n"
            channel=int(cmd[1])
            value=int(cmd[2])
            #print "channel: "+str(channel)+", value: "+str(value)
            return self.handle_pwm(channel,value)
                   
        return "Ok\n"

    
osm = OpenStopMotionPiControl()
    




def signal_term_handler(signum, frame):
    #setStatus(0,0,0)
    osm.stop_server()



if __name__ == "__main__":
    signal.signal(signal.SIGTERM, signal_term_handler)
    signal.signal(signal.SIGINT, signal_term_handler)
    
    
    osm.parse_args()
    osm.load_config()
    osm.init_gpio()
    osm.start_server()
    if have_rpi_gpio:
        GPIO.cleanup()
    sys.exit(0)

