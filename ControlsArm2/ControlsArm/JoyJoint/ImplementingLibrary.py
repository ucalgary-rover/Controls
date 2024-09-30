import pygame
from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
from Phidget22.Devices.DCMotor import * 
import traceback
import time


from shoulderLib import *
from elbowLib import *
from baseLib import *
from wristLib import *
#from clawLib import *
from wrist12Lib import *



VHubSerial_motors = 697103
VHubSerial_servo = 697066

leftSideDriveMotors = DCMotor()
rightSideDriveMotors = DCMotor()
leftWinchMotor = DCMotor()
bristleMotor =  DCMotor()

smoothing = 0.005

i = 0
index=0
jointMovement = ['shoulder', 'elbow', 'base','wrist main', 'wrist12', 'claw']
jointLenght = len(jointMovement)

def connect_motor(motor):
	try:
		motor.openWaitForAttachment(5000)
		print("Test connection")
	except:
		print("Failed to connect")


def onAttach_encoder(self):
	print("Encoder {0} attached!".format(self.getHubPort()))


try:
	# Functions to initialize components
	print(f"\nInitializing...\n\n")
	#initialize_motors()
	#shoulder_init(shoulderMotor, shoulderInfo)
	#elbow_init(elbowMotor, elbowInfo)
	print(f"\nSuccessfully initialized!\n")
	#leftSideDriveMotors.setHubPort(0)
	#rightSideDriveMotors.setHubPort(5)
	#leftWinchMotor.setHubPort(1)
	#bristleMotor.setHubPort(2)


	#connect_motor(leftSideDriveMotors)
	#init_motor(leftSideDriveMotors, 10)
	#connect_motor(rightSideDriveMotors)
	#init_motor(rightSideDriveMotors, 10)
	#connect_motor(leftWinchMotor)
	#init_motor(leftWinchMotor, 5)
	#connect_motor(bristleMotor)
	#init_motor(bristleMotor, 10)
	
	print("calling all library")
	shoulder_init()
	elbow_init()
	base_init()
	wrist_init() 
	#initialize_motors()
	wrist1_init()
	wrist2_init()
	

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
				
			elif event.button == 2:
				print("off")
			
			elif event.button == 3:
				if i == 0:
					print("Increase speed for shoulders")
					shoulderInfo[5] +=1
				elif i == 1:
					print("Increase speed for elbow")
					elbowInfo[5] +=1
				elif i == 2:
					print("Increase speed for base")
					baseInfo[5] +=1
				elif i == 3:
					print("Increase speed for wrist")
					wristInfo[5] +=1
				elif i == 4:
					print("Increase speed for wrist 1 and 2")
					wristInfo1[5] +=1
					wristInfo2[5] +=1
				#elif i == 5:
					#print("Increase speed for claw")
					#clawInfo[5] +=1
			elif event.button == 4:
				if i == 0:
					print("Decrease speed for shoulders")
					shoulderInfo[5] -=1
				elif i == 1:
					print("Decrease speed for elbow")
					elbowInfo[5] -=1
				elif i == 2:
					print("Decrease speed for base")
					baseInfo[5] -=1
				elif i == 3:
					print("Decrease speed for wrist")
					wristInfo[5] -=1
				elif i == 4:
					print("Decrease speed for wrist 1 and 2")
					wristInfo1[5] -=1
					wristInfo2[5] -=1
				#elif i == 5:
					#print("Decrease speed for claw")
					#clawInfo[5] -=1
				#exit()
			#elif event.button == 5:
				#claw_open()
				
			#elif event.button == 4:
				#claw_close()
		"""		
		if event.type == pygame.JOYBUTTONUP:
			if event.button == 5:
				#claw_off()
			elif event.button == 4:
				#claw_off()
		"""
		if event.type == pygame.JOYBUTTONUP:
			if event.button == 2:
				print("off")
				exit()
		if event.type == pygame.JOYAXISMOTION:
			print("Axis Motion:", event.axis, event.value)
		if event.type == pygame.JOYHATMOTION:
			print("Hat Motion:", event.hat, event.value)
			if jointMovement[i] == jointMovement[0]:
				if event.value ==(0,1):
					print("Shoulder goes up")
					shoulder_up()
				elif event.value == (0,-1):
					print("Shoulder goes down")
					shoulder_down() 

				else:
					print("Shoulder stays in motion")
					shoulder_off() 
					
			elif jointMovement[i] == jointMovement[1]:
				if event.value ==(0,1):
					print("Elbow goes up")
					elbow_up() 
				elif event.value == (0,-1):
					print("Elbow goes down")
					elbow_down() 

				else:
					print("Elbow stays in motion")
					elbow_off() 
		
	
			elif jointMovement[i] == jointMovement[2]:
				if event.value ==(0,1):
					print("Base goes up")
					base_up() 
				elif event.value == (0,-1):
					print("Base goes down")
					base_down() 

				else:
					print("Base stays in motion")
					base_off() 
					
					
			elif jointMovement[i] == jointMovement[3]:
				if event.value ==(0,1):
					print("Wrist goes up")
					wrist_up() 
				elif event.value == (0,-1):
					print("Wrist goes down")
					wrist_down() 

				else:
					print("Wrist stays in motion")
					wrist_off() 
					
			elif jointMovement[i] == jointMovement[4]:
				if event.value ==(0,1):
					print("Wrist 1 and 2 goes up")
					wrist12_up() 
				elif event.value == (0,-1):
					print("Wrist 1 and 2 goes down")
					wrist12_down() 

				else:
					print("Wrist 1 and 2 stays in motion")
					wrist12_off() 
							
			
			'''elif jointMovement[i] == jointMovement[5]:
				if event.value ==(0,1):
					print("claw opens")
					claw_open() 
				elif event.value == (0,-1):
					print("claw closes")
					claw_close() 
				else:
					print("claw stays in motion")
					claw_off() '''
		 


print("I'm done")

