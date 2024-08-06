from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
import traceback
import time

'''
elbow lib for initalization and movement:

function names:
elbow_init - initalizes elbow
elbow_open - opens elbow
elbow_close - closes elbow
elbow_off - elbow stays in motion 

'''


VHubSerial_motors = 697103
VHubSerial_servo = 697066

def connect_motor(motor):
    try:
        motor.openWaitForAttachment(5000)
        print("Test connection")
    except:
        print("Failed to connect")
        
elbowMotor = Stepper()
elbowInfo = [1.68, 1.68, 77, 1, 10, 10]
elbowInitialPos = 0
elbowAttached = False

def onAttach_motor(self):
    print(" {0} attached!".format(self.getHubPort()))

def onDetach(self):
    print("A motor detached!")

def onError(self,code, description):
    print("Code: " + ErrorEventCode.getName(code))
    print("Description: " + str(description))
    print("----------")

def elbow_init():
    global elbowMotor
    elbowMotor.setDeviceSerialNumber(VHubSerial_motors)
    elbowMotor.setHubPort(2)
    elbowMotor.setOnAttachHandler(onAttach_motor)
    elbowMotor.setOnDetachHandler(onDetach)
    elbowMotor.setOnErrorHandler(onError)
    try:
        elbowMotor.openWaitForAttachment(1000)
        print("Elbow attached")
    except:
        print("Elbow Motor not attached")
    if (elbowMotor.getAttached() == True):
        elbowMotor.setControlMode(StepperControlMode.CONTROL_MODE_RUN)
        elbowMotor.setCurrentLimit(elbowInfo[0])
        elbowMotor.setHoldingCurrentLimit(elbowInfo[1])
        elbowMotor.setRescaleFactor((1/16) * 1.8 * (1/elbowInfo[2]) * (1/elbowInfo[3]))
        elbowMotor.setAcceleration(elbowInfo[4])
        elbowMotor.setVelocityLimit(0)
        elbowMotor.setEngaged(True)
        elbowMotor.setDataInterval(elbowMotor.getMinDataInterval())

def elbow_up(speed):
    if not elbowMotor.getIsMoving(): 
        elbowMotor.setVelocityLimit(speed)
        
def elbow_down(speed):
    if not elbowMotor.getIsMoving(): 
        elbowMotor.setVelocityLimit(-speed)
    
def elbow_off():
    lim = elbowMotor.getVelocityLimit()
    elbowMotor.setVelocityLimit(lim * 3 / 4)
    time.sleep(0.005 / 4)
    elbowMotor.setVelocityLimit(lim / 2)
    time.sleep(0.005 / 4)
    elbowMotor.setVelocityLimit(lim / 4)
    time.sleep(0.005 / 4)
    elbowMotor.setVelocityLimit(0)


