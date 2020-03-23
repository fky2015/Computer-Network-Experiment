import sys
import logging
import serial
from utils.ConfigReader import ConfigReader
class ConnectException(Exception):
    pass

class Connector:
    def __init__(self,config:ConfigReader):
        self.config=config
        try:
            self.ser=serial.Serial(port=config.port,baudrate=config.baudrate,stopbits=config.stopbits,parity=config.parity)
        except Exception as e:
            print('[Error]: ', e)
        self.alive=True

    def __del__(self):
        try:
            self.ser.close()
        except Exception as e:
            print('[Error]: ', e)


    def Stop(self):
        self.alive=False

    def Read(self,bytenum:int=1):
        try:
            return self.ser.read(bytenum)
        except Exception as e:
            print('[Error]: ',e)

    def WriteStr(self,str):
        try:
            return self.ser.write(bytes(str+'\n',encoding='utf-8'))
        except Exception as e:
            print('[Error]: ',e)

    def ReadStr(self):
        try:
            return self.ser.readline().decode(encoding='utf-8')
        except Exception as e:
            print('[Error]: ', e)

    def Info(self):
        self.config.printconfig()






