import asyncio
from gbn import GBNProtocol
from configparser import ConfigParser

c = ConfigParser(default_section="default")
c.read('config.txt')


class GBNClientProtocol(GBNProtocol):
    FilterError = int(c['client']['FilterError'])
    FilterLost = int(c['client']['FilterLost'])


async def main():
    # Get a reference to the event loop as we plan to use
    # low-level APIs.
    loop = asyncio.get_running_loop()

    on_con_lost = loop.create_future()

    transport, _ = await loop.create_datagram_endpoint(
        lambda: GBNClientProtocol(
            loop, on_con_lost, print_tx=True, print_rx=True),
        remote_addr=('127.0.0.1', int(c['server']['UDPPort'])), local_addr=('127.0.0.1', int(c['client']['UDPPort'])))

    try:
        await on_con_lost
    finally:
        transport.close()


asyncio.run(main())
