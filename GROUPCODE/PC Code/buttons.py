from tkinter import *
#import RW
#import fileinfo_PC


global selected
global fileNames
global fileButtons
global volumeLevel

selected = ""
fileButtons = []
volumeLevel = 50

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
        file = open('testfile.txt', "r")
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
        print(fileNames[pointer],"selected!")
        selected = fileNames[pointer]

    def play(self):
        global selected
        if selected != "":
            print("Playing " + selected + "!")
##            sendData('P'+ selected)
        else:
            print("Playing!")

    def reverse(self):
        global selected
        if selected != "":
            print("Reversing " + selected + "...")
##            sendData('R'+ selected)
        else:
            print("Reversing...")


    def copy(self):
        global selected
        global fileNames
        global fileButtons
        if selected != "":
            print("Copying " + selected + "...")
            selected = input("What would you like the name of the copied file to be ?\n")
##            sendData('C'+ selected)
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
            sendData('D'+ selected)
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
##        sendData('V'+ "{0:0=3d}".format(volumeLevel))
        print("The volume is now at: " + str(volumeLevel) + "%")

    def quit(self):
        print("Quitting...")
        self.master.quit()
