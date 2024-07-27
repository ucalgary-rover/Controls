import asyncio
import websockets
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
from clawLib import * 
#this was commented from before idk why 
from wrist12Lib import *
from DriveJoyStick import *

VHubSerial_motors = 697103
VHubSerial_servo = 697066

#leftSideDriveMotors = DCMotor()
#rightSideDriveMotors = DCMotor()
leftWinchMotor = DCMotor()
bristleMotor =  DCMotor()

smoothing = 0.005

jointMovement = ['shoulder', 'elbow', 'base', 'wrist main', 'wrist12', 'claw', 'drive']
pos = 0.35
neg = -0.35

'''# Motor and Servo initialization
leftSideDriveMotors = DCMotor()
rightSideDriveMotors = DCMotor()
panoServo = RCServo()
'''

def connect_motor(motor):
    try:
        motor.openWaitForAttachment(5000)
        print("ROVER: Test connection")
    except:
        print("ROVER: Motor Failed to connect.")

def onAttach_encoder(self):
	print("ROVER: Encoder {0} attached!".format(self.getHubPort()))

'''def init_motor(motor, currentLimit):
    if motor.getAttached():
        motor.setCurrentLimit(currentLimit)
        motor.setTargetVelocity(0)
        motor.setAcceleration(5)'''

# Initialize motors
'''leftSideDriveMotors.setHubPort(0)
rightSideDriveMotors.setHubPort(5)
connect_motor(leftSideDriveMotors)
init_motor(leftSideDriveMotors, 7)
connect_motor(rightSideDriveMotors)
init_motor(rightSideDriveMotors, 7)

panoServo.setChannel(4)
panoServo.setHubPort(0)
panoServo.setDeviceSerialNumber(697066)
panoServo.openWaitForAttachment(5000)'''

try:
	# Functions to initialize components
	print(f"\nROVER: Initializing...\n\n")
	#initialize_motors()
	#shoulder_init(shoulderMotor, shoulderInfo)
	#elbow_init(elbowMotor, elbowInfo)
	print(f"\nROVER: Successfully initialized!\n")
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
	
	print("ROVER: calling all library")
	#shoulder_init()
	#elbow_init()
	#base_init()
	#wrist_init() 
	#initialize_motors()
	#wrist1_init()
	#wrist2_init()
	driver_init()

except PhidgetException as ex:
	traceback.print_exc()
	print()
	print("ROVER: PhidgetException " + str(ex.code) + " (" + ex.description + "): " + ex.details)
	print(f"ROVER: Successfully quit program.\n\nGoodbye!\n")

async def receive_commands(websocket, path):
	#idk why it wants me to re-define ts again
	smoothing = 0.005

	# Keep Drive as last value in list
	jointMovement = ['shoulder', 'elbow', 'base', 'wrist main', 'wrist12', 'claw', 'drive']
	pos = 0.35
	neg = -0.35
	
	async for message in websocket:
		#gets kinda annoying so comment out when code is finalized
		print("ROVER: Received:", message)
		parts = message.split(":")
		command = parts[0]
		movingJoint = parts[-1]
		
		if command == "button_down":
			button = int(parts[1])
			if button == 3:
				# Increase driver speed
				print("ROVER: Increasing speed")
				pos = pos + 0.1
				neg = neg - 0.1
				if pos > 1 or neg < -1:
					print("ROVER: Speed is more than 100% duty cycle, slow down!")
					pos = 1
					neg = -1

			elif button == 4:
				# Decrease driver speed
				print("ROVER: Decreasing speed")
				pos = pos - 0.1
				neg = neg + 0.1
				if pos < 0.2 or neg > 0.2:
					print("ROVER: speed is less than 20% Duty Cycle, speed up!")
					pos = 0.2
					neg = -0.2

		elif command == "button_up":
			button = int(parts[1])
			if button == 2:
				print("ROVER: off")
				exit()

		elif command == "axis_motion":
			axis = int(parts[1])
			value = float(parts[2])
			print("ROVER: Axis Motion:", axis, value)
			if jointMovement[movingJoint] == 'shoulder':
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Shoulder goes up")
						shoulder_up()	
					elif value > 0.3:
						print("ROVER: Shoulder goes down")
						shoulder_down()
					else:
						print("ROVER: Shoulder stays in motion")
						shoulder_off()

			elif jointMovement[movingJoint] == 'elbow':
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Elbow goes up")
						elbow_up() 
					elif value > 0.3:
						print("ROVER: Elbow goes down")
						elbow_down() 
					else:
						print("ROVER: Elbow stays in motion")
						elbow_off() 

			elif jointMovement[movingJoint] == 'base':
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Base goes up")
						base_up()	
					elif value > 0.3:
						print("ROVER: Base goes down")
						base_down()
					else:
						print("ROVER: Base stays in motion")
						base_off() 

			elif jointMovement[movingJoint] == 'wrist main':
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist goes up")
						wrist_up() 
					elif value > 0.3:
						print("ROVER: Wrist goes down")
						wrist_down() 
					else:
						print("ROVER: Wrist stays in motion")
						wrist_off() 

			elif jointMovement[movingJoint] == 'wrist12':
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist 1 and 2 goes up")
						wrist12_up() 	
					elif value > 0.3:
						print("ROVER: Wrist 1 and 2 goes down")
						wrist12_down() 
					else:
						print("ROVER: Wrist 1 and 2 stays in motion")
						wrist12_off() 
			elif jointMovement[movingJoint] == 'claw':
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: claw opens")
						claw_open() 
					elif value > 0.3:
						print("ROVER: claw closes")
						claw_close() 
					else:
						print("ROVER: claw stays in motion")
						claw_off()
			
			elif jointMovement[movingJoint] == 'drive':
				if axis == 0:  # X-axis (left-right)
					if value < -0.3:
						print("ROVER: Drive left")
						drive_left()
					elif value > 0.3:
						print("ROVER: Drive right")
						drive_right()
					else:
						print("ROVER: Drive stop")
						drive_stop()
				
				elif axis == 1:  # Y-axis (up-down)
					if value < -0.3:
						print("ROVER: Drive forward")
						drive_forward(pos, neg)
					elif value > 0.3:
						print("ROVER: Drive backward")
						drive_backward(pos, neg)
					else:
						print("ROVER: Drive stop")
						drive_stop()
		elif command == "hat_motion":
			hat = int(parts[1])
			value = float(parts[2])
			print("ROVER: Hat Motion:", hat, value)

print("ROVER: I'm done")

async def main():
	server = await websockets.serve(receive_commands, "0.0.0.0", 12345, ping_interval=None, ping_timeout=None)
	print("ROVER: WebSocket server started")
	await server.wait_closed()

asyncio.get_event_loop().run_until_complete(main())
'''
start_server = websockets.serve(receive_commands, "0.0.0.0", 12345)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
'''
