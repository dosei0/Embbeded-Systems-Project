import asyncio
import serial_asyncio


async def main(loop, messages):
    global R,W
    R,W = await serial_asyncio.open_serial_connection(url='/dev/ttyACM0', baudrate=9600)

async def send(w, msgs):
    while True:
        for _ in range(len(msgs)):
            m = msgs.pop()
            print("SEND:", m)
            w.write(m)
        await asyncio.sleep(0.5)

async def recv(r):
    msg = await r.readuntil(b'\n\r')
    while True:
        msg = await r.readuntil(b'\n\r')
        while msg[0] == 0:
            msg = msg[1:]
        #print(msg)
        try:
            msg = msg.decode("ASCII")[:-2]
        except UnicodeDecodeError:
            return
        msg.replace("\x00","")
        l = msg.split(" ")
        while '' in l: l.remove('')
        cmd, args = l[0], l[1:]
        if cmd in CALLBACKS:
            # Execute callback
            CALLBACKS[cmd](*l)
        else:
            print("DEBUG: ", l)

async def randomSend(msgs):
    while True:
        await asyncio.sleep(2)
        msgs.append("/test;".encode())

R,W = None,None
messages = []
CALLBACKS = {}
loop = asyncio.get_event_loop()
loop.run_until_complete(main(loop, messages))

if __name__ == "__main__":
    
    CALLBACKS= {
        "A:": lambda *args: args #print(*args) #Update UI here
    }

    
    loop.run_until_complete(asyncio.gather(
        recv(R),
        send(W,messages),
        randomSend(messages)
    ))

    loop.close()


