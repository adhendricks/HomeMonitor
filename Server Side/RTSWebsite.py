from flask import Flask, render_template
import serial
#import time
#import thread

app = Flask(__name__)
port = serial.Serial("/dev/rfcomm0",baudrate = 9600)   

rawdata = " "

#def thread1():
#    rawdata = " "
#    while True:
#        if port.inWaiting():
#            rawdata = port.read(21) #22 ch buffer limit
#            port.flushInput()
#        time.sleep(5.5)
@app.route('/')

def index():
    rawdata = " "
    if port.inWaiting():
    #if rawdata!= " ":
        rawdata = port.read(21) #22 ch buffer limit
        port.flushInput()
        splitdata = rawdata.decode("utf-8").split(" ")
        tempT = splitdata[0]
        humT = splitdata[1]
        lightT =splitdata[2].lstrip("0")
        motionT = splitdata[3]
        if motionT[0] == '1':
            motionT = "Detected"
        if motionT[0] == '0':
            motionT = "Not detected"

    if rawdata != " ":
       return render_template('index.html', temp = tempT, humid = humT, light = lightT, motion =motionT)
    return """
        <head>
        <meta http-equiv="refresh" content="6">
        </head>
        <h1> Waiting for RTrackify data... </h1>
        """

if __name__ == '__main__':
    #thread.start_new_thread(thread1)
    app.run(host='0.0.0.0')
