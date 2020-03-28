import enum
import itertools
from typing import Optional, List
from time import sleep
import pickle

MAX_SEQ = 7


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def crc_gen(message):
    # CRC-16-CITT poly, the CRC sheme used by ymodem protocol
    poly = 0x1021
    # 16bit operation register, initialized to zeros
    reg = 0xFFFF
    # pad the end of the message with the size of the poly
    message += '\x00\x00'
    # for each bit in the message
    for byte in message:
        mask = 0x80
        while(mask > 0):
            # left shift by one
            reg <<= 1
            # input the next bit from the message into the right hand side of the op reg
            if ord(byte) & mask:
                reg += 1
            mask >>= 1
            # if a one popped out the left of the reg, xor reg w/poly
            if reg > 0xffff:
                # eliminate any one that popped out the left
                reg &= 0xffff
                # xor with the poly, this is the remainder
                reg ^= poly
    return reg


class FrameType(enum.Enum):
    DATA = 1
    ACK = 2
    NACK = 3


class Frame:
    def __init__(self, info: int, seq: int, ack: int, kind: FrameType = FrameType.DATA):
        self.kind = kind
        self.seq = seq
        self.ack = ack
        self.info = info
        self.CRC = ""

    def get_info(self):
        return f"message: {self.info}"

    def __str__(self):
        return f"[Seq: {self.seq} " + \
            f"Ack: {self.ack} " + \
            f"Data: {self.info}" \
            + "]"

    def __repr__(self):
        return self.__str__()


class Data:
    MAX_SEQ = 7

    def __init__(self, timer):
        self.frame_gen = itertools.cycle(range(MAX_SEQ+1))
        self.index_gen = itertools.count(0)
        # [(data, if_get_ack)]
        self.buffer = []
        self.timer = timer
        self.ack_expect = 0
        self.next_frame_to_send = next(self.frame_gen) % (self.MAX_SEQ + 1)
        self.frame_expected = 0

    def crc_gen(self, message: Frame) -> int:
        message = message.get_info()
        return crc_gen(message)

    def verify(self, message: Frame, crc: int) -> bool:
        return self.crc_gen(message.get_info()) == crc

    def _get_next_frame_from_nextwork_layer(self):
        temp = self.next_frame_to_send
        self.next_frame_to_send = next(self.frame_gen) % (self.MAX_SEQ + 1)
        return Frame(next(self.index_gen), temp, (self.frame_expected + MAX_SEQ) % (self.MAX_SEQ + 1))

    def __iter__(self):
        return self

    def __next__(self) -> Frame:
        if len(self.buffer) < self.MAX_SEQ - 1:
            f = self._get_next_frame_from_nextwork_layer()
            timer = self.timer()
            f.CRC = self.crc_gen(f)
            self.buffer.append((f, False, timer))
            return f
        raise StopIteration

    def get_next_batch_frame_to_send(self) -> List[Frame]:
        return [i for i in self]

    def _between(self, a, b, c) -> bool:
        return True if a <= b < c or c < a <= b or b < c < a else False

    def receive_an_ack(self, f: Frame):
        while self._between(self.ack_expect, f.ack, self.next_frame_to_send):
            self.ack_expect = (self.ack_expect + 1) % (self.MAX_SEQ + 1)
            x, *self.buffer = self.buffer
            x[2].cancel()



class GBNProtocol:
    TRANSMISSION_DELAY = 0.1
    PROPAGATION_DELAY = 0.1
    FilterError = 20
    FilterLost = 20
    TIMEOUT = 4

    def __init__(self, loop, on_con_lost, print_rx=False, print_tx=False):
        self.loop = loop
        self.on_con_lost = on_con_lost
        self.transport = None
        self.data = Data(lambda: loop.call_later(
            self.TIMEOUT, self.timeout, loop))
        self.next_filter_error = self.FilterError
        self.next_filter_lost = self.FilterLost + 5
        self.print_tx = print_tx
        self.print_rx = print_rx

    def _send_filter(self, data: Frame) -> Optional[Frame]:
        if data.kind == FrameType.DATA and data.info == self.next_filter_error:
            # create an error
            invalid_frame = Frame(data.info, data.seq, data.ack)
            invalid_frame.CRC = data.CRC
            invalid_frame.info = -1
            data = invalid_frame
            self.next_filter_error += self.FilterError
            if self.print_tx:
                print(bcolors.WARNING, "[Send] [error]:", data, bcolors.ENDC)
        elif data.kind == FrameType.DATA and data.info == self.next_filter_lost:
            # loss
            if self.print_tx:
                print(bcolors.FAIL, "[Send] [loss]:", data, bcolors.ENDC)
            self.next_filter_lost += self.FilterLost
            return None
        else:
            if self.print_tx:
                print(bcolors.HEADER, "[Send] [normal]:", data, bcolors.ENDC)
        return data

    def timeout(self, loop):
        [t[2].cancel() for t in self.data.buffer[1:]]
        self.data.buffer = [(t[0], t[1], loop.call_later(
            self.TIMEOUT, self.timeout, loop)) for t in self.data.buffer]

        for (message, *_) in self.data.buffer:
            if self.print_tx:
                print(
                    bcolors.OKBLUE,
                    f"\n[Send][Timeout] ack_expect: {self.data.ack_expect}",
                    f"next_frame_to_resend: {message}",
                    f"frame_expected: {self.data.frame_expected}",
                    bcolors.ENDC)
            self.send(message)

    def send(self, data: Frame, addr=None):
        filtered = self._send_filter(data)
        if filtered is not None:
            sleep(self.TRANSMISSION_DELAY)
            # register a callback
            self.transport.sendto(pickle.dumps(filtered), addr)

    def connection_made(self, transport, addr=None):
        self.transport = transport
        for message in self.data.get_next_batch_frame_to_send():
            if self.print_tx:
                print(
                    f"\n[Send] ack_expect: {self.data.ack_expect}",
                    f"next_frame_to_send: {self.data.next_frame_to_send}",
                    f"frame_expected: {self.data.frame_expected}")
            self.send(message, addr)

    def datagram_received(self, data, addr):
        sleep(self.PROPAGATION_DELAY)

        message = pickle.loads(data)

        # if we should accept frame as data
        if self.data.frame_expected == message.seq:
            if message.CRC != crc_gen(message.get_info()):
                if self.print_rx:
                    print(bcolors.FAIL, f"\n[Recv] {message}", bcolors.ENDC,
                          f"frame_expected: {self.data.frame_expected}\nack_expected: {self.data.ack_expect}")
                    print("error frame!")
            else:
                self.data.frame_expected = (
                    self.data.frame_expected + 1) % (self.data.MAX_SEQ + 1)

        self.data.receive_an_ack(message)

        print(f"\n[Recv] {message}",
              f"frame_expected: {self.data.frame_expected}\nack_expected: {self.data.ack_expect}")
        # print([i[0].seq for i in self.data.buffer])

        try:
            message = next(self.data)
            sleep(self.TRANSMISSION_DELAY)
            print(
                f"\n[Send] ack_expect: {self.data.ack_expect}",
                f"next_frame_to_send: {self.data.next_frame_to_send}",
                f"frame_expected: {self.data.frame_expected}")
            self.send(message, addr)
        except:
            pass

    def error_received(self, exc):
        print('Error received:', exc)

    def connection_lost(self, exc):
        print("Connection closed")
        if self.on_con_lost is not None:
            self.on_con_lost.set_result(True)


if __name__ == "__main__":
    pass
