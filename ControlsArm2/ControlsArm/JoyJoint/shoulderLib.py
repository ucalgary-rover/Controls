from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
import traceback
import time

'''
Shoulder lib for initalization and movement:

function names:
shoulder_init - initalizes shoulder
shoulder_open - opens shoulder
shoulder_close - closes shoulder
shoulder_off - shoulder stays in motion 

'''

VHubSerial_motors = 697103
VHubSerial_servo = 697066

def connect_motor(motor):
    try:
        motor.openWaitForAttachment(5000)
        print("Test connection")
    except:
        print("Failed to connect")

shoulderMotor = Stepper()
shoulderInfo = [3, 1.68, 100, 1, 5, 5]
shoulderInitialPos = 0
shoulderAttached = False


def onAttach_motor(self):
    print(" {0} attached!".format(self.getHubPort()))

def onDetach(self):
    print("A motor detached!")

def onError(self,code, description):
    print("Code: " + ErrorEventCode.getName(code))
    print("Description: " + str(description))
    print("----------")

def shoulder_init():
    global shoulderMotor
    shoulderMotor.setDeviceSerialNumber(VHubSerial_motors)
    shoulderMotor.setHubPort(1)
    shoulderMotor.setOnAttachHandler(onAttach_motor)
    shoulderMotor.setOnDetachHandler(onDetach)
    shoulderMotor.setOnErrorHandler(onError)
    try:
        shoulderMotor.openWaitForAttachment(1000)
        print("Shoulder attached")
    except:
        print("Shoulder Motor not attached")
    if (shoulderMotor.getAttached() == True):
        shoulderMotor.setControlMode(StepperControlMode.CONTROL_MODE_RUN)
        shoulderMotor.setCurrentLimit(shoulderInfo[0])
        shoulderMotor.setHoldingCurrentLimit(shoulderInfo[1])
        shoulderMotor.setRescaleFactor((1/16) * 1.8 * (1/shoulderInfo[2]) * (1/shoulderInfo[3]))
        shoulderMotor.setAcceleration(shoulderInfo[4])
        shoulderMotor.setVelocityLimit(0)
        shoulderMotor.setEngaged(True)
        shoulderMotor.setDataInterval(shoulderMotor.getMinDataInterval())

def shoulder_up():
    if not shoulderMotor.getIsMoving():
        shoulderMotor.setVelocityLimit(shoulderInfo[5])

def shoulder_down():
    if not shoulderMotor.getIsMoving():
        shoulderMotor.setVelocityLimit(-shoulderInfo[5])

def shoulder_off():
    lim = shoulderMotor.getVelocityLimit()
    shoulderMotor.setVelocityLimit(lim * 3 / 4)
    time.sleep(0.005 / 4)
    shoulderMotor.setVelocityLimit(lim / 2)
    time.sleep(0.005 / 4)
    shoulderMotor.setVelocityLimit(lim / 4)
    time.sleep(0.005 / 4)
    shoulderMotor.setVelocityLimit(0)


