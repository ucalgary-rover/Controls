from motor__init import *
import threading
import os

sizeS = len(motors_S)
sizeDC = len(motors)
CDetected = True
inputDC = [False]*sizeDC
inputS = [False]*sizeS
connectionDC = [False]*sizeDC
connectionS = [False]*sizeS
groundDC = [False]*sizeDC
groundS = [False]*sizeS

def inputVoltage(number, type):
    try:
        first = type[number].getVoltage()
        time.sleep(1)
        second = type[number].getVoltage()
        
        if(type == motors):
            prev = inputDC[number]
            if abs(second - first < 1) and not(22 <= second <= 26):
                inputDC[number] = False
<<<<<<< HEAD
        elif(type == motors_S):
=======
                if prev != inputDC[number]: CDetected = True
                
        elif(type == motors_S):
            prev = inputS[number]
>>>>>>> e9a3fd8ffc82ffabdf20b71f2e566600be0775c2
            if abs(second - first < 1) and not(11.5 <= second <= 12.5):
                inputS[number] = False
                if prev != inputS[number]: CDetected = True


    except AttributeError:
        if type == motors:
            inputDC[number] = True
            if prev != inputDC[number]: CDetected = True
        else:
            inputS[number] = True
            if prev != inputS[number]: CDetected = True

# the two other functions you need to see if there is a change or not
def turnedOnConnectionServo(number):
    try:
        prev = connectionS[number]
        if motors_S[number].getAttached():
<<<<<<< HEAD
            print("The Servo motor #%d is attached"% number)
            if motors_S[number].getIsOpen():
                print("The Servo motor #%d is open"% number)
=======
            if motors_S[number].getIsOpen():
>>>>>>> e9a3fd8ffc82ffabdf20b71f2e566600be0775c2
                connectionS[number] = True
                if prev != connectionS[number]: CDetected = True
                
            else:
<<<<<<< HEAD
                print("The Servo motor #%d is not open" % number)
=======
>>>>>>> e9a3fd8ffc82ffabdf20b71f2e566600be0775c2
                motors_S[number].openWaitForAttachment(1000)
                connectionS[number] = False
                if prev != connectionS[number]: CDetected = True
            inputVoltage(number, motors_S)
            groundVoltage(number, motors_S)

    except AttributeError:
         connectionS[number] = False
         if prev != connectionDC[number]: CDetected = Trues

def turnedOnConnectionDC(number):
    try:
        prev = connectionDC[number]
        if motors[number].getAttached():
            print("The DC motor #%d is attached"% number)
            if motors[number].getIsOpen():
                print("The DC motor #%d is open"% number)
                connectionDC[number] = True
                if prev != connectionDC[number]: CDetected = True
            else:
                print("The DC motor #%d is not open" % number)
                motors[number].openWaitForAttachment(1000)
                connectionDC[number] = False
                if prev != connectionDC[number]: CDetected = True
            inputVoltage(number, motors)
            groundVoltage(number,motors)

    except AttributeError:
        connectionDC[number] = False
        if prev != connectionDC[number]: CDetected = True

def groundVoltage(number,volt):
    x = 1 

def printer(): 
    if CDetected:
        print("DC MOTOR STATS: ")
        for x in range(sizeDC):
            print("DC MOTOR #",x)
            print("---------------")
            print("Input voltage status: " + ("FAULTY!!!!!!" if inputDC[x] == False else "VALID"))
            print("Connection status:"+ (" FAILED " if connectionDC[x] == False else " SUCCESS"))

        for y in range(sizeS):
            print("Servo MOTOR #",y)
            print("---------------")
            print("Input voltage status: " + ("FAULTY!!!!!!" if inputS[y] == False else "VALID"))
            print("Connection status:" + (" FAILED " if connectionS[y] == False else " SUCCESS"))
        time.sleep(1)
        CDetected = False 

<<<<<<< HEAD
def pollerSystem():
    while True:
        #  clear the terminal
        os.system('cls' if os.name == 'nt' else 'clear')

        threads_servo = [threading.Thread(target=turnedOnConnectionServo, args=(x,)) for x in range(len(motors_S))]
        for threadS in threads_servo:
            threadS.start()

        threads_dc = [threading.Thread(target=turnedOnConnectionDC, args=(y,)) for y in range(len(motors))]
        for thread in threads_dc:
            thread.start()

        for threadSS in threads_servo:
            threadSS.join()

        for threadd in threads_dc:
            threadd.join()

        time.sleep(1)

        print("DC MOTOR STATS: ")
        for x in range(sizeDC):
            print("DC MOTOR #"+x)
            print("---------------")
            print("Input voltage status: " + ("FAULTY!!!!!!" if inputDC[x] == False else "Correct"))
            print("Connection status:"+ (" FAILED " if connectionDC[x] == False else " SUCCESS"))

        for y in range(sizeS):
            print("Servo MOTOR #"+ y)
            print("---------------")
            print("Input voltage status: " + ("FAULTY!!!!!!" if inputS[x] == False else "Correct"))
            print("Connection status:" + (" FAILED " if connectionS[x] == False else " SUCCESS"))

        time.sleep(1)
=======

def pollerSystem():
    CDetected = True
    while True:
        #  clear the terminal
        os.system('cls' if os.name == 'nt' else 'clear')

        threads_servo = [threading.Thread(target=turnedOnConnectionServo, args=(x,)) for x in range(len(motors_S))]
        for threadS in threads_servo:
            threadS.start()

        threads_dc = [threading.Thread(target=turnedOnConnectionDC, args=(y,)) for y in range(len(motors))]
        for thread in threads_dc:
            thread.start()

        for threadSS in threads_servo:
            threadSS.join()
>>>>>>> e9a3fd8ffc82ffabdf20b71f2e566600be0775c2

        for threadd in threads_dc:
            threadd.join()

        time.sleep(1)
        printer()
