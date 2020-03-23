from configparser import ConfigParser
class ConfigReader:
    def __init__(self,ConfigFile='./config/connect.conf'):
        try:
            self.config=ConfigParser()
            self.config.read(ConfigFile,encoding='UTF-8')
            self.port=self.config.get('Device','SerialPort')
            self.baudrate=self.config.getint('Device','BaudRate')
            self.databit=self.config.getint('Code','DataBits')
            self.parity=self.config.get('Code','Parity')
            self.stopbits=self.config.getfloat('Code','StopBits')
        except Exception as e:
            print('[Error]: ',e)

    def printconfig(self):
        print('------------------------------------------------------')
        print('SerialPort:\t',self.port)
        print('BaudRate:\t',self.baudrate)
        print('DataBits:\t',self.databit)
        print('Parity:\t',self.parity)
        print('StopBits:\t',self.stopbits)
        print('------------------------------------------------------')
