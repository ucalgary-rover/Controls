import pygame
from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
from Phidget22.Devices.DCMotor import * 
import traceback
import time

VHubSerial_motors = 697103
VHubSerial_servo = 697066

leftSideDriveMotors = DCMotor()
rightSideDriveMotors = DCMotor()
leftWinchMotor = DCMotor()
bristleMotor =  DCMotor()

smoothing = 0.005

i = 0
index=0
jointMovement = ['shoulder', 'elbow', 'wrist main', 'wrist1', 'wrist2', 'claw']
jointLenght = len(jointMovement)

def connect_motor(motor):
	try:
		motor.openWaitForAttachment(5000)
		print("Test connection")
	except:
		print("Failed to connect")


def init_motor(motor, currentLimit):
	if motor.getAttached():
		motor.setDeviceSerialNumber(697270)
		motor.setCurrentLimit(currentLimit)
		motor.setTargetVelocity(0)
		motor.setAcceleration(5)
		motor.setFanMode(FanMode.FAN_MODE_ON)#FAN_MODE_AUTO


  
shoulderMotor = Stepper()
shoulderInfo = [3, 1.68, 100, 1, 5, 5]
shoulderInitialPos = 0
shoulderAttached = False

def onAttach_motor(self):
	print(" {0} attached!".format(self.getHubPort()))
	shoulderAttached = True

def onAttach_encoder(self):
	print("Encoder {0} attached!".format(self.getHubPort()))


def onDetach(self):
	print("A motor detached!")


def onError(self,code, description):
	print("Code: " + ErrorEventCode.getName(code))
	print("Description: " + str(description))
	print("----------")


def shoulder_init(shoulderMotor, shoulderInfo):
	shoulderMotor.setDeviceSerialNumber(VHubSerial_motors)
	shoulderMotor.setHubPort(1)
	shoulderMotor.setOnAttachHandler(onAttach_motor)
	shoulderMotor.setOnDetachHandler(onDetach)
	shoulderMotor.setOnErrorHandler(onError)
	try:
		shoulderMotor.openWaitForAttachment(1000)
		print("shoulder attached")
	except:
		print("Shoulder Motor not attached")
	if (shoulderMotor.getAttached() == True):
		shoulderMotor.setControlMode(StepperControlMode.CONTROL_MODE_RUN)
		shoulderMotor.setCurrentLimit(shoulderInfo[0])
		shoulderMotor.setHoldingCurrentLimit(shoulderInfo[1])
		shoulderMotor.setRescaleFactor((1/16) * 1.8 * (1/shoulderInfo[2]) * (1/shoulderInfo[3])) # (1/16) * Step angle * (1/Gearbox ratio) * (1/Gear ratio) p
		shoulderMotor.setAcceleration(shoulderInfo[4])
		shoulderMotor.setVelocityLimit(0)
		shoulderMotor.setEngaged(True)
		shoulderMotor.setDataInterval(shoulderMotor.getMinDataInterval())


try:
	# Functions to initialize components
	print(f"\nInitializing...\n\n")
	#initialize_motors()
	shoulder_init(shoulderMotor, shoulderInfo)
	print(f"\nSuccessfully initialized!\n")
	leftSideDriveMotors.setHubPort(0)
	rightSideDriveMotors.setHubPort(5)
	leftWinchMotor.setHubPort(1)
	bristleMotor.setHubPort(2)


	connect_motor(leftSideDriveMotors)
	init_motor(leftSideDriveMotors, 10)
	connect_motor(rightSideDriveMotors)
	init_motor(rightSideDriveMotors, 10)
	connect_motor(leftWinchMotor)
	init_motor(leftWinchMotor, 5)
	connect_motor(bristleMotor)
	init_motor(bristleMotor, 10)

except PhidgetException as ex:
	traceback.print_exc()
	print()
	print("PhidgetException " + str(ex.code) + " (" + ex.description + "): " + ex.details)
	print(f"Successfully quit program.\n\nGoodbye!\n")

pygame.init()
pygame.joystick.init()
joystick_count = pygame.joystick.get_count()
print(joystick_count)

joystick = pygame.joystick.Joystick(0)
joystick.init()

print("Initialized joystick", joystick.get_name())

while True:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			pygame.quit()	
			exit()
		if event.type == pygame.JOYBUTTONDOWN:
			print("Button Pressed:", event.button)
			if event.button == 9:
				i = (i + 1)%jointLenght
				print(jointMovement[i])
			elif event.button == 8:
				if i == 0:
					i = jointLenght - 1
					jointMovement[jointLenght-1]
				else:
					i-=1
				print(jointMovement[i])
		if event.type == pygame.JOYAXISMOTION:
			print("Axis Motion:", event.axis, event.value)
		if event.type == pygame.JOYHATMOTION:
			print("Hat Motion:", event.hat, event.value)
			if event.value ==(0,1):
				print("Shoulder goes up")
				if not shoulderMotor.getIsMoving():
					shoulderMotor.setVelocityLimit(shoulderInfo[5])
			elif event.value == (0,-1):
				print("Shoulder goes down")
				if not shoulderMotor.getIsMoving():
					shoulderMotor.setVelocityLimit(-shoulderInfo[5])

			else:
				print("Shoulder stays in motion")
				lim = shoulderMotor.getVelocityLimit()
				shoulderMotor.setVelocityLimit(lim * 3 / 4)
				time.sleep(smoothing / 4)
				shoulderMotor.setVelocityLimit(lim / 2)
				time.sleep(smoothing / 4)
				shoulderMotor.setVelocityLimit(lim / 4)
				time.sleep(smoothing / 4)
				shoulderMotor.setVelocityLimit(0)


print("I'm done")

