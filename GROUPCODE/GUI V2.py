from tkinter import *
import asyncio
import serial_asyncio

global selected
global fileNames
global fileButtons
global volumeLevel

selected = ""
fileNames = []
fileButtons = []
volumeLevel = 50
start = 0
end = 0
serial_received = '' #store string of data received from MBED
serial_sent = '' #string of data to be sent to the MBED
command = '/60;'
class Window(Frame):

    def __init__(self, master):

        Frame.__init__(self, master)
        self.master = master
        self.init_window()

    def init_window(self):

        self.master.title("MP3 Player")    # Windows Title
        self.pack(fill = BOTH, expand =1)

        menu = Menu(self.master)
        self.master.config(menu=menu)

        file = Menu(menu)

        file.add_command(label = "Import", command = self.Import)
        menu.add_cascade(label = "File", menu = file)

        quit = Menu(menu)

        quit.add_command(label = "Quit", command = self.quit)
        menu.add_cascade(label = "Quit", menu =  quit)

        playButton = Button(self, text="Play", fg = "green", command=self.play)
        playButton.place(x=10,y=30)

        reverseButton = Button(self, text="Reverse", fg = "pink", command=self.reverse)
        reverseButton.place(x=50,y=30)

        volumeAdjustButton = Button(self, text="Volume Adjust", fg = "orange", command=self.volumeAdjust)
        volumeAdjustButton.place(x=115,y=30)

        copyButton = Button(self, text="Copy", fg = "blue", command=self.copy)
        copyButton.place(x=220,y=30)

        deleteButton = Button(self, text="Delete", fg = "red", command=self.delete)
        deleteButton.place(x=270,y=30)

    def fileButtons(self):
        global fileNames
        global fileButtons
        ycoord = 80
        print(fileButtons)
        print(fileNames)
        for x in range(0, len(fileNames)):
            fileButtons.append(fileNames[x])
        print(fileButtons)
        for x in range (0, len(fileNames)):
            fileButtons[x] = Radiobutton(self, text= fileNames[x], variable= var, value = x, fg = "purple", command=self.fileselect)
            fileButtons[x].place(x=10, y= ycoord)
            ycoord += 40

    def Import(self):
        global fileNames
        global fileButtons
        print("Importing...")
        file = open('text.txt', "r")
        z = 0
        for line in file:
                fileNames.append(line.strip())
                z +=1
        file.close()

        self.fileButtons()

    def fileselect(self):
        global selected
        global fileNames
        pointer = var.get()
        print(fileNames[point])

    def play(self):
        global selected
        if selected != "":
            print("Playing " + selected + "!")
        else:
            print("Playing!")

    def reverse(self):
        global selected
        if selected != "":
            print("Reversing " + selected + "...")
        else:
            print("Reversing...")

    def copy(self):
        global selected
        global fileNames
        global fileButtons
        if selected != "":
            print("Copying " + selected + "...")
            selected = input("What would you like the name of the copied file to be ?\n")
            fileNames.append(selected)
            for x in range (0, len(fileButtons)):
                fileButtons[x].destroy()
            fileButtons = []
            self.fileButtons()

        else:
            print("Copying...")

    def delete(self):
        global selected
        global fileNames
        global fileButtons
        if selected != "":
            print("Deleting " + selected + "...")
            fileNames.remove(selected)
            print(fileNames)
            for x in range (0, len(fileButtons)):
                fileButtons[x].destroy()
            fileButtons = []
            self.fileButtons()
        else:
            print("Playing!")

    def volumeAdjust(self):
        global volumeLevel
        print("The current volume is at: " + str(volumeLevel) + "%")
        volumeLevel = input("What would you like the volume level percentage to be?\n")
        global command
        command = '/' + str(volumeLevel) + ';'
        sendData(volumeLevel)
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(asyncio.new_event_loop())
        asyncio.get_event_loop()
        coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyACM0', baudrate=9600)
        loop.run_until_complete(coro)
        loop.run_forever()
        loop.close()
        #asyncio.get_event_loop().stop()

        print("The volume is now at: " + str(volumeLevel) + "%")

    def quit(self):
        print("Quitting...")
        self.master.quit()

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
            serial_received = ''
            self.transport.close()
        if ('/'+serial_received +';') == command:
            serial_received =''
            self.transport.close()


    def connection_lost(self, exc):
        print('port closed')
        print(exc)
        asyncio.get_event_loop().stop()

def sendData(data):
    global serial_sent
    serial_sent = '/' + data + ';'





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

root = Tk()
var = IntVar()
root.geometry("500x500")
app = Window(root)
root.mainloop()

root.destroy()

# while True:
#     loop = asyncio.get_event_loop()
#     coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyACM0', baudrate=9600)
#     loop.run_until_complete(coro)
#     loop.run_forever()
#     loop.close()
