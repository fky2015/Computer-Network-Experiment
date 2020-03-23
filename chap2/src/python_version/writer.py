from utils import ConfigReader
from utils import Connector
import serial


import serial.tools.list_ports

import serial

import serial.tools.list_ports


if __name__=='__main__':
    config=ConfigReader('./config/writer.conf')
    connect=Connector(config)
    connect.Info()
    # write to the serial
    connect.WriteStr(input('please type the input: \n'))
