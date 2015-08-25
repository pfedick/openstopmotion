import ConfigParser
import os

class Config(object):
    """!Object to read an hold the configuration parameters"""
    
    class RGBPins(object):
        """!Object to hold the GPIO-Pins for RGB-Status-LED"""
        def __init__(self, red=0, green=0, blue=0):
            self.red=red
            self.green=green
            self.blue=blue
            
    class MotorPins(object):
        """!"""
        def __init__(self):
            self.name = ""
            self.dir = 0
            self.step = 0
            self.sleep = 0
            self.stopsensor = 0
            
    
    def __init__(self):
        # IP or Hostname the server should bind on
        self.hostname = "localhost"
        # Port
        self.port = 9090
        # I2C address of Adafruit PWM Servo Driver
        self.pwm_i2c_address=0x40
        # PWM frequency
        self.pwm_frequency = 1000
        # GPIO Pins of RGB-Status-LED
        self.StatusRGBPins = Config.RGBPins(8,10,12)
        # GPIO Pins for Motor 1
        self.Motor1 = Config.MotorPins()
        # GPIO Pins for Motor 2
        self.Motor2 = Config.MotorPins()
        
    @staticmethod
    def __open_config__(filename):
        """!Opens the configuration file for reading and returns a ConfigParser object"""
        filename=os.path.abspath(filename)
        config=ConfigParser.RawConfigParser()
        if len(config.read(filename)) == 0:
            raise IOError("could not load configuration file: "+str(filename))
        return config
    
    @staticmethod
    def __read_param__(config, section, key, default_value):
        try:
            return config.get(section,key)
        except BaseException:
            return default_value
        
    def __read_server_param__(self, config):
        self.hostname = self.__read_param__(config, "server","hostname","localhost")
        self.port = int(self.__read_param__(config, "server","port",9090))

    def __read_pwm_param__(self, config):
        self.pwm_i2c_address = int(self.__read_param__(config, "pwm","i2c_address",64))
        self.pwm_frequency = int(self.__read_param__(config, "pwm","frequency",1000))

    def __read_statusled_param__(self, config):
        self.StatusRGBPins.red = int(self.__read_param__(config, "statusrgb","red",0))
        self.StatusRGBPins.green = int(self.__read_param__(config, "statusrgb","green",0))
        self.StatusRGBPins.blue = int(self.__read_param__(config, "statusrgb","blue",0))

    def __read_motor_param__(self, config, section):
        motor=Config.MotorPins()
        motor.name = self.__read_param__(config, section,"name",section)
        motor.dir = int(self.__read_param__(config, section,"dir",0))
        motor.step = int(self.__read_param__(config, section,"step",0))
        motor.sleep = int(self.__read_param__(config, section,"sleep",0))
        motor.stopsensor = int(self.__read_param__(config, section,"stopsensor",0))

        
    def load(self, filename):
        """!Loads the configuration file"""
        if (not filename):
            raise ValueError
        if not os.path.exists(filename):
            raise IOError("Configuration file not found: "+str(filename))
        config = self.__open_config__(filename)
        self.__read_server_param__(config)
        self.__read_pwm_param__(config)
        self.__read_statusled_param__(config)
        self.Motor1 = self.__read_motor_param__(config,"motor1")
        self.Motor2 = self.__read_motor_param__(config,"motor2")


