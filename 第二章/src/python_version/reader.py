from utils import ConfigReader
from utils import Connector
import serial


import serial.tools.list_ports

import serial

import serial.tools.list_ports


if __name__=='__main__':
    config=ConfigReader('./config/reader.conf')
    connect=Connector(config)
    connect.Info()
    # read the serial
    print(connect.ReadStr())
