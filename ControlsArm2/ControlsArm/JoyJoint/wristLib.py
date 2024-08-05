from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
import traceback
import time

'''
Wrist lib for initalization and movement:

function names:
wrist_init - initalizes wrist 
wrist_open - opens wrist
wrist_close - closes wrist
wrist_off - wrist stays in motion 

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

wristMotor = Stepper() 
wristInfo = [1.68, 1.68, 51, 1, 15, 10]
wristInitialPos = 0
wristAttached = False


def onAttach_motor(self):
    print(" {0} attached!".format(self.getHubPort()))

def onDetach(self):
    print("A motor detached!")

def onError(self,code, description):
    print("Code: " + ErrorEventCode.getName(code))
    print("Description: " + str(description))
    print("----------")

def wrist_init():
	global wristMotor
	wristMotor.setDeviceSerialNumber(VHubSerial_motors) 
	wristMotor.setHubPort(3) #idk
	wristMotor.setOnAttachHandler(onAttach_motor) 
	wristMotor.setOnDetachHandler(onDetach) 
	wristMotor.setOnErrorHandler(onError) 
	try:
		wristMotor.openWaitForAttachment(1000) # If having motor connection timout issues, increase this number 
		print("wrist attached")
	except: 
		print("wrist Motor not attached")
	if (wristMotor.getAttached() == True): 
		wristMotor.setControlMode(StepperControlMode.CONTROL_MODE_RUN) 
		wristMotor.setCurrentLimit(wristInfo[0]) 
		wristMotor.setHoldingCurrentLimit(wristInfo[1])
		wristMotor.setRescaleFactor((1/16) * 1.8 * (1/wristInfo[2]) * (1/wristInfo[3])) # (1/16) * Step angle * (1/Gearbox ratio) * (1/Gear ratio) p
		wristMotor.setAcceleration(wristInfo[4])
		wristMotor.setVelocityLimit(0) 
		wristMotor.setEngaged(True) 
		wristMotor.setDataInterval(wristMotor.getMinDataInterval())


def wrist_up():
    if not wristMotor.getIsMoving():
        wristMotor.setVelocityLimit(wristInfo[5])

def wrist_down():
    if not wristMotor.getIsMoving():
        wristMotor.setVelocityLimit(-wristInfo[5])

def wrist_off():
    lim = wristMotor.getVelocityLimit()
    wristMotor.setVelocityLimit(lim * 3 / 4)
    time.sleep(smoothing/ 4)
    wristMotor.setVelocityLimit(lim / 2)
    time.sleep(smoothing / 4)
    wristMotor.setVelocityLimit(lim / 4)
    time.sleep(smoothing/ 4)
    wristMotor.setVelocityLimit(0)



