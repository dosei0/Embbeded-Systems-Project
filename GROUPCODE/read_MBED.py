import asyncio
import serial_asyncio


start = 0
end = 0
serial_received = '' #store string of data received from MBED
command = '/test;'
serial_sent = ''  #string of data to be sent to the MBED

##Use of asynchronous communication for transmission of data, without the use of an external clock signal, where data can be transmitted intermittently rather than in a steady stream.
class Output(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
        global serial_sent
        print('port opened')
        transport.serial.rts = True
        #transport.write('hello world\n\r'.encode())
        if serial_sent != '':
            transport.write(serial_sent.encode())
            print(serial_sent)
            serial_sent = ''


    def data_received(self, data):
        global serial_received
        serial_received += data.decode()
        print('data received', serial_received)
        if 'End of List' in serial_received:
            self.transport.close()
        if ('/'+serial_received +';') == command:
            self.transport.close()


    def connection_lost(self, exc):
        print('port closed')
        print(exc)
        asyncio.get_event_loop().stop()

def sendData(data):
    global serial_sent
    serial_sent = '/' + data + ';'

sendData('test')
loop = asyncio.get_event_loop()
coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyACM0', baudrate=9600)
loop.run_until_complete(coro)
loop.run_forever()
loop.close()





def displayFiles():
    files = []#list of files on SD card
    file = []
    characters = ''
    for line in serial_received:
        characters += line
        file = characters.split("/")

    print(file)
    start = file[0].find("Start of List")
    end = file[0].find("End of List")




    file = file[start+3:end]


    for i in range(len(file)):

        files.append(file[i][:-2])

    print(start, end)

    print(files)
    f = open("text.txt","w")
    f.write(str(files))


displayFiles()
