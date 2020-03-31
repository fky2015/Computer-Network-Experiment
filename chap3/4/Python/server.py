import asyncio
from gbn import GBNProtocol
from configparser import ConfigParser


c = ConfigParser(default_section="default")
c.read('config.txt')


class GBNServerProtocol(GBNProtocol):
    FilterError = int(c['server']['FilterError'])
    FilterLost = int(c['server']['FilterLost'])

    def connection_made(self, transport):
        self.transport = transport


async def main():
    print("Starting UDP server")

    # Get a reference to the event loop as we plan to use
    # low-level APIs.
    loop = asyncio.get_running_loop()

    # One protocol instance will be created to serve all
    # client requests.
    transport, _ = await loop.create_datagram_endpoint(
        lambda: GBNServerProtocol(loop, None, print_rx=True, print_tx=True),
        local_addr=('127.0.0.1', int(c['server']['UDPPort'])), remote_addr=('127.0.0.1', int(c['client']['UDPPort'])), allow_broadcast=True)

    try:
        await asyncio.sleep(3600)  # Serve for 1 hour.
    finally:
        transport.close()


asyncio.run(main())
