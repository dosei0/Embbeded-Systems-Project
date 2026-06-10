from tkinter import *
import asyncio
import serial_asyncio
import random
import time
from read_MBED_V2 import *


global volumeLevel
global size
global circle
global size_canvx
global size_canvy


class Window(Frame):

    def __init__(self, master,bg = "black"):

        Frame.__init__(self, master, bg = "black")
        self.master = master
        self.init_window()


    def init_window(self):

        global circle
        self.master.title("Audio Visualiser")    # Windows Title
        self.pack(fill = BOTH, expand =1)

        size = 0

        global canvas
        canvas = Canvas(self, width = 1000, height = 1000,highlightthickness=0)

        circle = canvas.create_oval(10, 10, 100, 100, outline="#0000FF",
            fill="#0000FF", width=2)

        canvas.place(x=633, y = 350)
        canvas.config(background = "black")

##
   # def pulse(self):
   #     start_time = time.time()
   #     if time.time() - start_time >= audio_length-10:
   #         canvas.config(background = "white")
   #         self.master.configure(background="white")
    def audioLevel(self, volumeLevel):
           volumeLevel = abs(int(volumeLevel)/(2**15/100))
           print(volumeLevel)
           global size
           global circle
           global size_canvx
           global size_canvy

           if volumeLevel > 700:
               size = 700
           else:
               size = volumeLevel

##
           global canvas
           size_canvx =-1/3*(size)
           size_canvy =-4/10*(size)

           canvas.move(circle, size_canvx,0)   #  for x += 10
           canvas.move(circle, 0, size_canvy)
           canvas.coords(circle,10,10, 100+(size),100+(size))
           colours = ["green", "blue", "red", "orange", "violet", "yellow", "indigo"]
           colour = random.choice(colours)
           canvas.itemconfig(circle, outline=colour, fill = colour)



root = Tk()
var = IntVar()
root.geometry("2000x2000")
app = Window(root)

CALLBACKS["A:"] = lambda *v: app.audioLevel(v[1])



async def run_tk(root, interval=0.05):
    try:
        while True:
            root.update()
            await asyncio.sleep(interval)
    except TclError as e:
        if "application has been destroyed" not in e.args[0]:
            raise

#loop = asyncio.get_event_loop()
#loop.run_until_complete(main(loop, messages))
print(R,W)
loop.run_until_complete(asyncio.gather(
    recv(R),
    send(W,messages),
    run_tk(root)
))

loop.close()
