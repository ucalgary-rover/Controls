import asyncio
import websockets
from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
from Phidget22.Devices.DCMotor import * 
import traceback
import time
import msgpack

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

# jointMovement = ['base', 'shoulder', 'elbow', 'wrist main', 'wrist12', 'claw', 'drive']
driveSpeed = 0.6
armSpeed = 5

# keep last state to not add too many events to queue
driveControlerLastMovingJoint = -1
armControlerLastMovingJoint = -1

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
	shoulder_init()
	elbow_init()
	base_init()
	wrist_init() 
	# initialize_motors()
	wrist1_init()
	wrist2_init()
	driver_init()
	claw_init()

except PhidgetException as ex:
	traceback.print_exc()
	print()
	print("ROVER: PhidgetException " + str(ex.code) + " (" + ex.description + "): " + ex.details)
	print(f"ROVER: Successfully quit program.\n\nGoodbye!\n")

async def receive_commands(websocket, path):
	#idk why it wants me to re-define ts again
	smoothing = 0.005

	# Keep Drive as last value in list
	oldJointStop = [ lambda : base_off(), lambda : shoulder_off(), lambda : elbow_off(), lambda : wrist_off(), lambda : wrist12_off(), lambda : claw_off(), lambda : drive_stop()]
	global driveSpeed
	global armSpeed

	global driveControlerLastMovingJoint
	global armControlerLastMovingJoint
	
	async for message in websocket:
		#gets kinda annoying so comment out when code is finalized
		print("ROVER: Received:", message)
		parts = msgpack.unpackb(message, raw=False)
		command = parts[0]
		
		if command == 0: # 0 is for button down
			button = parts[1]
			controllerId = parts[2]
			if button == 5:
				# Increase driver speed
				if (controllerId == 0):
					print("ROVER: Increasing driveSpeed")
					driveSpeed = driveSpeed + 0.1
					if driveSpeed > 1:
						print("ROVER: driveSpeed is more than 100% duty cycle, slow down!")
						driveSpeed = 1
					print(f"driveSpeed : {driveSpeed * 100}%\n")
				elif (controllerId == 1):
					print("ROVER: Increasing armSpeed")
					armSpeed = armSpeed + 1
					if armSpeed > 10:
						print("ROVER: armSpeed is more than 100% duty cycle, slow down!")
						armSpeed = 10
					print(f"armSpeed : {armSpeed * 10}%\n")

			elif button == 4:
				# Decrease driver speed
				if (controllerId == 0):
					print("ROVER: Decreasing driveSpeed")
					driveSpeed = driveSpeed - 0.1
					if driveSpeed < 0.2:
						print("ROVER: driveSpeed is less than 20% Duty Cycle, driveSpeed up!")
						driveSpeed = 0.2
					print(f"driveSpeed : {driveSpeed * 100}%\n")
				if (controllerId == 1):
					print("ROVER: Decreasing armSpeed")
					armSpeed = armSpeed - 1
					if armSpeed < 2:
						print("ROVER: armSpeed is less than 20% Duty Cycle, armSpeed up!")
						armSpeed = 2
					print(f"armSpeed : {armSpeed * 10}%\n")

			else:
				print("button not used")

		elif command == 1: # 1 is for button up
			button = parts[1]
			if button == 2:
				print("ROVER: off")
				exit()

		elif command == 2: # 2 is for axis_motion
			movingJoint = int(parts[-1])
			# if the joint changes, the make sure that the old joint turns offdrive will never be the old joint as it is separate
			if movingJoint != armControlerLastMovingJoint:
				oldJointStop[armControlerLastMovingJoint]()
				print("Turning off old Joint")
			axis = parts[1]
			value = parts[2]
			print("ROVER: Axis Motion:", axis, value)
			if movingJoint == 1: # 1 is for shoulder
				if axis == 3: # Y-axis
					if value > 0.3:
						print("ROVER: Shoulder goes up")
						shoulder_up(armSpeed)	
					elif value < -0.3:
						print("ROVER: Shoulder goes down")
						shoulder_down(armSpeed)
					else:
						print("ROVER: Shoulder stays in motion")
						shoulder_off()
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 2: # 2 is for elbow
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Elbow goes up")
						elbow_up(armSpeed) 
					elif value > 0.3:
						print("ROVER: Elbow goes down")
						elbow_down(armSpeed) 
					else:
						print("ROVER: Elbow stays in motion")
						elbow_off() 
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 0: # 0 is for base
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Base goes up")
						base_up(armSpeed)	
					elif value > 0.3:
						print("ROVER: Base goes down")
						base_down(armSpeed)
					else:
						print("ROVER: Base stays in motion")
						base_off() 
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 3: # 3 is for wrist main
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist goes up")
						wrist_up(armSpeed) 
					elif value > 0.3:
						print("ROVER: Wrist goes down")
						wrist_down(armSpeed) 
					else:
						print("ROVER: Wrist stays in motion")
						wrist_off() 
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 4: # 4 is for wrist12
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist 1 and 2 goes up")
						wrist12_up(armSpeed) 	
					elif value > 0.3:
						print("ROVER: Wrist 1 and 2 goes down")
						wrist12_down(armSpeed) 
					else:
						print("ROVER: Wrist 1 and 2 stays in motion")
						wrist12_off() 
					armControlerLastMovingJoint = movingJoint
					continue
				if axis == 2: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist 1 and 2 goes left")
						wrist12_left(armSpeed) 	
					elif value > 0.3:
						print("ROVER: Wrist 1 and 2 goes right")
						wrist12_right(armSpeed) 
					else:
						print("ROVER: Wrist 1 and 2 stays in motion")
						wrist12_off()
					armControlerLastMovingJoint = movingJoint
					continue
			elif movingJoint == 5: # 5 is for claw
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
					armControlerLastMovingJoint = movingJoint
					continue
			
			elif movingJoint == 6: # 6 is for drive
				if axis == 0:  # X-axis (left-right)
					if value < -0.3:
						print("ROVER: Drive left")
						drive_left(driveSpeed)
					elif value > 0.3:
						print("ROVER: Drive right")
						drive_right(driveSpeed)
					else:
						print("ROVER: Drive stop")
						drive_stop()
					driveControlerLastMovingJoint = movingJoint
					continue
				
				elif axis == 1:  # Y-axis (up-down)
					if value < -0.3:
						print("ROVER: Drive forward")
						drive_forward(driveSpeed)
					elif value > 0.3:
						print("ROVER: Drive backward")
						drive_backward(driveSpeed)
					else:
						print("ROVER: Drive stop")
						drive_stop()
					driveControlerLastMovingJoint = movingJoint
					continue
		# elif command == "hat_motion":
		# 	hat = int(parts[1])
		# 	value = float(parts[2])
		# 	print("ROVER: Hat Motion:", hat, value)

print("ROVER: I'm done")

async def main():
	print("oof")
	server = await websockets.serve(receive_commands, "0.0.0.0", 12345, ping_interval=None, ping_timeout=None)
	print("ROVER: WebSocket server started")
	await server.wait_closed()

asyncio.get_event_loop().run_until_complete(main())
'''
start_server = websockets.serve(receive_commands, "0.0.0.0", 12345)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
'''
