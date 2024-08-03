from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
import traceback
import time

'''
Base lib for initalization and movement:

function names:
base_init - initalizes base
base_open - opens base
base_close - closes base
base_off - base stays in motion 
'''

VHubSerial_motors = 697103
VHubSerial_servo = 697066

smoothing = 0.005

def connect_motor(motor):
    try:
        motor.openWaitForAttachment(5000)
        print("Test connection")
    except:
        print("Failed to connect")

baseMotor = Stepper() 
baseInfo = [1.68, 1.68, 100, 1, 10, 10]
baseInitialPos = 0
BaseAttached = False


def onAttach_motor(self):
    print(" {0} attached!".format(self.getHubPort()))

def onDetach(self):
    print("A motor detached!")

def onError(self,code, description):
    print("Code: " + ErrorEventCode.getName(code))
    print("Description: " + str(description))
    print("----------")


def base_init(): 
    global baseMotor
    baseMotor.setDeviceSerialNumber(VHubSerial_motors) 
    baseMotor.setHubPort(0) #idk
    baseMotor.setOnAttachHandler(onAttach_motor) 
    baseMotor.setOnDetachHandler(onDetach) 
    baseMotor.setOnErrorHandler(onError) 
    try: 
        baseMotor.openWaitForAttachment(1000) # If having motor connection timout issues, increase this number 
        print("Base attached")
    except: 
        print("Base Motor not attached")
    if (baseMotor.getAttached() == True): 
        baseMotor.setControlMode(StepperControlMode.CONTROL_MODE_RUN) 
        baseMotor.setCurrentLimit(baseInfo[0]) 
        baseMotor.setHoldingCurrentLimit(baseInfo[1])
        baseMotor.setRescaleFactor((1/16) * 1.8 * (1/baseInfo[2]) * (1/baseInfo[3])) # (1/16) * Step angle * (1/Gearbox ratio) * (1/Gear ratio) p
        baseMotor.setAcceleration(baseInfo[4])
        baseMotor.setVelocityLimit(0) 
        baseMotor.setEngaged(True) 
        baseMotor.setDataInterval(baseMotor.getMinDataInterval())

def base_up():
    if not baseMotor.getIsMoving():
        baseMotor.setVelocityLimit(baseInfo[5])

def base_down():
    if not baseMotor.getIsMoving():
        baseMotor.setVelocityLimit(-baseInfo[5])

def base_off():
    lim = baseMotor.getVelocityLimit()
    baseMotor.setVelocityLimit(lim * 3 / 4)
    time.sleep(smoothing/ 4)
    baseMotor.setVelocityLimit(lim / 2)
    time.sleep(smoothing / 4)
    baseMotor.setVelocityLimit(lim / 4)
    time.sleep(smoothing/ 4)
    baseMotor.setVelocityLimit(0)



