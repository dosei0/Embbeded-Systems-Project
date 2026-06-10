import RW
files = []#list of files on SD card
start = 0
end = 0
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
