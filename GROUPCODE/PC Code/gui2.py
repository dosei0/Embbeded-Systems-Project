from buttons import Window
import RW
import asyncio
import serial_asyncio
import fileinfo_PC
from read_MBED import Output
from tkinter import *

def main():
    root = Tk()
    var = IntVar()
    root.geometry("500x500")
    app = Window(root)
    root.mainloop()
    root.destroy()
##    loop = asyncio.get_event_loop()
##    coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyACM0', baudrate=9600)
##    loop.run_until_complete(coro)
##    loop.run_forever()
##    loop.close()
