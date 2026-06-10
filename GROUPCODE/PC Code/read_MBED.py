import asyncio
import serial_asyncio


start = 0
end = 0
serial_received = '' #store string of data received from MBED
serial_sent = '/test\n\r;' #string of data to be sent to the MBED


##Use of asynchronous communication for transmission of data, without the use of an external clock signal, where data can be transmitted intermittently rather than in a steady stream.
class Output(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
        global serial_sent
        print('port opened')
        transport.serial.rts = False
        #transport.write('hello world\n\r'.encode())
        if serial_sent != '':
            transport.write(serial_sent.encode())
            print(serial_received)
            #serial_sent = ''


    def data_received(self, data):
        global serial_received
        serial_received += data.decode()
        print('data received', serial_received)
        if 'End of List' in serial_received:
            self.transport.close()
        if ('/'+serial_received +';') == serial_sent:
            self.transport.close()


    def connection_lost(self, exc):
        print('port closed')
        print(exc)
        asyncio.get_event_loop().stop()
