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
from argparse import ArgumentParser

from config import Config

try:
    import RPi.GPIO as GPIO
    have_rpi_gpio = True
    from Adafruit_PWM_Servo_Driver import PWM
except ImportError:
    have_rpi_gpio = False
    

class OpenStopMotionPiControl(object):
    def __init__(self):
        self.config=Config()
        self.args = None
        
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
        

class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):

    def handle(self):
        data = self.request.recv(1024)
        cur_thread = threading.current_thread()
        response = "{}: {}".format(cur_thread.name, data)
        self.request.sendall(response)

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass


def signal_term_handler(signum, frame):
    #setStatus(0,0,0)
    if have_rpi_gpio:
        GPIO.cleanup()
    sys.exit(0)



if __name__ == "__main__":
    signal.signal(signal.SIGTERM, signal_term_handler)
    
    osm = OpenStopMotionPiControl()
    osm.parse_args()
    osm.load_config()
    


