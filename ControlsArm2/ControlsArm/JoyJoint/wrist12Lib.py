from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
import traceback
import time



VHubSerial_motors = 697103
VHubSerial_servo = 697066

smoothing = 0.005


def connect_motor(motor):
    try:
        motor.openWaitForAttachment(5000)
        print("Test connection")
    except:
        print("Failed to connect")


wristMotor1 = Stepper() 
wristInfo1 = [.67, .67, 100, 1, 15, 10]
wristInitialPos1 = 0
    
wristMotor2 = Stepper() 
wristInfo2 = [.67, .67, 100, 1, 15, 10]
wristInitialPos2 = 0

#motors = [wrist1Motor, wrist2Motor] 
#motorsInfo = [wrist1Info, wrist2Info] 



def onAttach_motor(self):
    print(" {0} attached!".format(self.getHubPort()))

def onDetach(self):
    print("A motor detached!")

def onError(self,code, description):
    print("Code: " + ErrorEventCode.getName(code))
    print("Description: " + str(description))
    print("----------")


'''def initialize_motors(): 
    global motors, motorsInfo
    for i in range(len(motors)): 
        motors[i].setDeviceSerialNumber(VHubSerial_motors) 
        motors[i].setHubPort(i) 
        motors[i].setOnAttachHandler(onAttach_motor) 
        motors[i].setOnDetachHandler(onDetach) 
        motors[i].setOnErrorHandler(onError) 
        try: 
            motors[i].openWaitForAttachment(1000) # If having motor connection timout issues, increase this number 
            print("wrist 1 and 2 attached")
        except: 
            print(" " + str(i) + " not attached")
        if (motors[i].getAttached() == True): 
            motors[i].setControlMode(StepperControlMode.CONTROL_MODE_RUN) 
            motors[i].setCurrentLimit(motorsInfo[i][0]) 
            motors[i].setHoldingCurrentLimit(motorsInfo[i][1])
            motors[i].setRescaleFactor((1/16) * 1.8 * (1/motorsInfo[i][2]) * (1/motorsInfo[i][3])) # (1/16) * Step angle * (1/Gearbox ratio) * (1/Gear ratio) p
            motors[i].setAcceleration(motorsInfo[i][4])
            motors[i].setVelocityLimit(0) 
            motors[i].setEngaged(True) 
            motors[i].setDataInterval(motors[i].getMinDataInterval())'''


def wrist1_init():
	global wristMotor1
	wristMotor1.setDeviceSerialNumber(VHubSerial_motors) 
	wristMotor1.setHubPort(4) 
	wristMotor1.setOnAttachHandler(onAttach_motor) 
	wristMotor1.setOnDetachHandler(onDetach) 
	wristMotor1.setOnErrorHandler(onError) 
	try:
		wristMotor1.openWaitForAttachment(1000) # If having motor connection timout issues, increase this number 
		print("wrist  1 attached")
	except: 
		print("wrist Motor 1 not attached")
	if (wristMotor1.getAttached() == True): 
		wristMotor1.setControlMode(StepperControlMode.CONTROL_MODE_RUN) 
		wristMotor1.setCurrentLimit(wristInfo1[0]) 
		wristMotor1.setHoldingCurrentLimit(wristInfo1[1])
		wristMotor1.setRescaleFactor((1/16) * 1.8 * (1/wristInfo1[2]) * (1/wristInfo1[3])) # (1/16) * Step angle * (1/Gearbox ratio) * (1/Gear ratio) p
		wristMotor1.setAcceleration(wristInfo1[4])
		wristMotor1.setVelocityLimit(0) 
		wristMotor1.setEngaged(True) 
		wristMotor1.setDataInterval(wristMotor1.getMinDataInterval())
		


def wrist2_init():
	global wristMotor2
	wristMotor2.setDeviceSerialNumber(VHubSerial_motors) 
	wristMotor2.setHubPort(5) 
	wristMotor2.setOnAttachHandler(onAttach_motor) 
	wristMotor2.setOnDetachHandler(onDetach) 
	wristMotor2.setOnErrorHandler(onError) 
	try:
		wristMotor2.openWaitForAttachment(1000) # If having motor connection timout issues, increase this number 
		print("wrist 2 attached")
	except: 
		print("wrist Motor 2 not attached")
	if (wristMotor2.getAttached() == True): 
		wristMotor2.setControlMode(StepperControlMode.CONTROL_MODE_RUN) 
		wristMotor2.setCurrentLimit(wristInfo2[0]) 
		wristMotor2.setHoldingCurrentLimit(wristInfo2[1])
		wristMotor2.setRescaleFactor((1/16) * 1.8 * (1/wristInfo2[2]) * (1/wristInfo2[3])) # (1/16) * Step angle * (1/Gearbox ratio) * (1/Gear ratio) p
		wristMotor2.setAcceleration(wristInfo2[4])
		wristMotor2.setVelocityLimit(0) 
		wristMotor2.setEngaged(True) 
		wristMotor2.setDataInterval(wristMotor2.getMinDataInterval())



def wrist12_up():
	if not wristMotor1.getIsMoving() and not wristMotor2.getIsMoving(): 
		wristMotor1.setVelocityLimit(wristInfo1[5]) 
		wristMotor2.setVelocityLimit(wristInfo2[5]) 

def wrist12_down():
	if not wristMotor1.getIsMoving() and not wristMotor2.getIsMoving(): 
		wristMotor1.setVelocityLimit(-wristInfo1[5]) 
		wristMotor2.setVelocityLimit(-wristInfo2[5]) 
	
def wrist12_off():
	lim = wristMotor1.getVelocityLimit() 
	wristMotor1.setVelocityLimit(lim * 3 / 4) 
	time.sleep(smoothing / 4) 
	wristMotor1.setVelocityLimit(lim / 2) 
	time.sleep(smoothing / 4) 
	wristMotor1.setVelocityLimit(lim / 4) 
	time.sleep(smoothing / 4) 
	wristMotor1.setVelocityLimit(0)
	lim = wristMotor2.getVelocityLimit() 
	wristMotor2.setVelocityLimit(lim * 3 / 4) 
	time.sleep(smoothing / 4) 
	wristMotor2.setVelocityLimit(lim / 2) 
	time.sleep(smoothing / 4) 
	wristMotor2.setVelocityLimit(lim / 4) 
	time.sleep(smoothing / 4) 
	wristMotor2.setVelocityLimit(0)




