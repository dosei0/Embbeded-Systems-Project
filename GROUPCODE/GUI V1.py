from tkinter import *
from read_MBED_V2 import *
from graph import *

class Window(Frame):

    def __init__(self, master, serialCmds):

        Frame.__init__(self, master)
        self.master = master
        self.init_window()
        self.cmds = serialCmds

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

        reverseButton = Button(self, text="Reverse", fg = "black", command=self.reverse)
        reverseButton.place(x=65,y=30)

        copyButton = Button(self, text="Copy", fg = "blue", command=self.copy)
        copyButton.place(x=145,y=30)

        deleteButton = Button(self, text="Delete", fg = "red", command=self.delete)
        deleteButton.place(x=205,y=30)

        self.volumeSlider = Scale(self, from_=100, to=0)
        self.volumeSlider.set(0)
        self.volumeSlider.place(x=300, y=30)

        volumeChange = Button(self, text="Change Volume", fg = "purple", command= self.volumeAdjust)
        volumeChange.place(x=350, y= 30)

        global canvas
        canvas = Canvas(self, width = 100, height = 400)

        y1 = 230
        y2 = 250

        global boxes
        for x in range(0, 10):
            box = canvas.create_rectangle(10, y1, 60 ,y2, outline = "black", fill = "white", width = 2)
            boxes.append(box)
            y1 -= 20
            y2 -= 20

        canvas.place(x=500, y = 0)

    def volumeIntensityGraph(self, volumeLevel):
        if len(boxes) < 10:
            return

        try:
            volumeLevel = int(volumeLevel)
            #print(int(volumeLevel/(2**15 / 10)))
        except:
            return

        global canvas

        y1 = 230
        y2 = 250

        for x in range(0, 10):
            canvas.itemconfig(boxes[x], fill = "white")
            y1 -= 20
            y2 -= 20

        y1 = 230
        y2 = 250

        for x in range(0, int(volumeLevel/(2**15 / 10))):
            if x < 4:
                fill = "green"
            elif x > 6:
                fill = "red"
            else:
                fill = "yellow"

            canvas.itemconfig(boxes[x], fill = fill)
            y1 -= 20
            y2 -= 20

        root.update()

    def volumeAdjust(self):
        volumeLevel = self.volumeSlider.get()
        print("The volume is now at: " + str(volumeLevel) + "%")
        self.sendCommand("V",str(volumeLevel).zfill(3))
        self.volumeIntensityGraph(volumeLevel)


    def fileButtons(self):
        global fileNames
        global fileButtons
        ycoord = 80
        for x in range (0, len(fileButtons)):
            fileButtons[x].destroy()
        fileButtons = []
        #print(fileButtons)
        #print(fileNames)
        for x in range(0, len(fileNames)):
            fileButtons.append(fileNames[x])
        #print(fileButtons)
        for x in range (0, len(fileNames)):
            fileButtons[x] = Radiobutton(self, text= fileNames[x], variable= var, value = x, fg = "purple", command=self.fileselect)
            fileButtons[x].place(x=10, y= ycoord)
            ycoord += 40

    def Import(self):
        self.sendCommand("F")

    def fileselect(self):
        global selected
        global fileNames
        pointer = var.get()
        print(fileNames[pointer],"selected!")
        selected = fileNames[pointer]

    def play(self):
        global selected
        if selected != "":
            print("Playing " + selected + "!")
            self.sendCommand("P",selected)
        else:
            print("Playing!")

    def reverse(self):
        global selected
        if selected != "":
            print("Reversing " + selected + "...")
            self.sendCommand("R",selected)
        else:
            print("Reversing...")

    def copy(self):
        global selected
        global fileNames
        global fileButtons
        if selected != "":
            print("Copying " + selected + "...")
            newFile = input("What would you like the name of the copied file to be ?\n")
            fileNames.append(newFile)
            for x in range (0, len(fileButtons)):
                fileButtons[x].destroy()
            fileButtons = []
            self.fileButtons()
            self.sendCommand("C", selected, newFile)
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
            print(fileButtons)
            self.fileButtons()
            self.sendCommand("D",selected)
        else:
            print("Playing!")

    def quit(self):
        print("Quitting...")
        self.destroy()

    def sendCommand(self, prefix, infile=False, outfile=False):
        cmd = prefix + "\0"
        if infile:
            cmd += infile + "\0"
        if outfile:
            cmd += outfile + "\0"
        cmd = "/" + cmd + ";"
        self.cmds.append(cmd.encode())


global selected
global fileNames
global fileButtons
global volumeLevel
global wavLevels
global boxes

selected = ""
fileNames = []
fileButtons = []
volumeLevel = 50
wavLevels = []
boxes = []

root = Tk()
var = IntVar()
root.geometry("900x900")
app = Window(root, messages)

fileList = []

CALLBACKS["A:"] = lambda *v: app.volumeIntensityGraph(v[1])
CALLBACKS["CLRFILS"] = lambda *v: fileNames.clear()
CALLBACKS["F:"] = lambda *v: fileNames.append(v[1])
CALLBACKS["ENDFILS"] = lambda *v: app.fileButtons()
CALLBACKS["CLRVALS"] = lambda *v: wavLevels.clear()
CALLBACKS["V:"] = lambda *v: wavLevels.append(int(v[1]))
CALLBACKS["ENDVALS"] = lambda *v: display_graph(wavLevels)

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

#root.mainloop()
#root.destroy()
