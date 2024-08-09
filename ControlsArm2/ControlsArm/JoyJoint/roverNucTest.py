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

jointMovement = ['base', 'shoulder', 'elbow', 'wrist main', 'wrist12', 'claw', 'drive']
speed = 0.6
#jointMovementdrive = ['drive', 'tool_left', 'tool_middle', 'tool_right' ]
#pos = 0.35
#neg = -0.35

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
'''

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
	jointMovement = [ 'base', 'shoulder', 'elbow', 'wrist main', 'wrist12', 'claw', 'drive']
		#jointMovementdrive = ['drive', 'tool_left', 'tool_middle', 'tool_right']
	pos = 0.35
	neg = -0.35
	
	async for message in websocket:
		#gets kinda annoying so comment out when code is finalized
		print("ROVER: Received:", message)
		parts = message.split(":")
		command = parts[0]
		movingJoint = int(parts[-1])
		#movingJointdrive = int(parts[-2])
		
		if command == "button_down":
			button = int(parts[1])
			if button == 5:
				# Increase driver speed
				print("ROVER: Increasing speed")
				pos = pos + 0.1
				neg = neg - 0.1
				if pos > 1 or neg < -1:
					print("ROVER: Speed is more than 100% duty cycle, slow down!")
					pos = 1
					neg = -1
				print(f"pos : {pos}\nneg : {neg}")

			elif button == 4:
				# Decrease driver speed
				print("ROVER: Decreasing speed")
				pos = pos - 0.1
				neg = neg + 0.1
				if pos < 0.2 or neg > 0.2:
					print("ROVER: speed is less than 20% Duty Cycle, speed up!")
					pos = 0.2
					neg = -0.2
				print(f"pos : {pos}\nneg : {neg}")
			elif button == 2:
				if button == 2:
					print("ROVER: bristles on")
				#bristleMotor.setTargetVelocity(-1)
	
			else:
				print("button not used")

		elif command == "button_up":
			button = int(parts[1])
			if button == 2:
				print("ROVER: bristles off")
				#bristleMotor.setTargetVelocity(0)
				

		elif command == "axis_motion":
			axis = int(parts[1])
			value = float(parts[2])
			print("ROVER: Axis Motion:", axis, value)
			if jointMovement[movingJoint] == 'shoulder':
				if axis == 3: # Y-axis
					if value > 0.3:
						print("ROVER: Shoulder goes up")
						shoulder_up()	
					elif value < -0.3:
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
				if axis == 0: # x-axis
					if value < -0.3:
						print("ROVER: Wrist 1 and 2 goes left")
						wrist12_left() 	
					elif value > 0.3:
						print("ROVER: Wrist 1 and 2 goes right")
						wrist12_right() 
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
			
			'''
			elif jointMovementdrive[movingJointdrive] == 'drive':
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
			
			elif jointMovementdrive[movingJointdrive] == 'tool_left':
				if axis == 3:  # Y-axis (up-down)
					if value < -0.3:
						print("ROVER: left tool goes up")
						tool_left_up()
					elif value > 0.3:
						print("ROVER: left tool goes down")
						tool_left_down()
					else:
						print("ROVER: left tool stop")
						tool_left_stop()
			
			elif jointMovementdrive[movingJointdrive] == 'tool_middle':
				if axis == 3:  # Y-axis (up-down)
					if value < -0.3:
						print("ROVER: middle tool goes up")
						tool_middle_up()
					elif value > 0.3:
						print("ROVER: middle tool goes down")
						tool_middle_down()
					else:
						print("ROVER: middle tool stop")
						tool_middle_stop()
						
			elif jointMovementdrive[movingJointdrive] == 'tool_right':
				if axis == 3:  # Y-axis (up-down)
					if value < -0.3:
						print("ROVER: right tool goes up")
						tool_right_up()
					elif value > 0.3:
						print("ROVER: right tool goes down")
						tool_right_down()
					else:
						print("ROVER: right tool stop")
						tool_right_stop()
						'''
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

#
'''
start_server = websockets.serve(receive_commands, "0.0.0.0", 12345)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
'''

'''
async def receive_commands(websocket, path):
	#idk why it wants me to re-define ts again
	smoothing = 0.005

	# Keep Drive as last value in list
	oldJointStop = [ lambda : base_off(), lambda : shoulder_off(), lambda : elbow_off(), lambda : wrist_off(), lambda : wrist12_off(), lambda : claw_off(), lambda : drive_stop()]
	global speed

	global driveControlerLastMovingJoint
	global armControlerLastMovingJoint
	
	async for message in websocket:
		#gets kinda annoying so comment out when code is finalized
		print("ROVER: Received:", message)
		parts = msgpack.unpackb(message, raw=False)
		command = parts[0]
		
		if command == 0: # 0 is for button down
			button = parts[1]
			if button == 5:
				# Increase driver speed
				print("ROVER: Increasing speed")
				speed = speed + 0.1
				if speed > 1:
					print("ROVER: Speed is more than 100% duty cycle, slow down!")
					speed = 1
				print(f"speed : {speed * 100}%\n")

			elif button == 4:
				# Decrease driver speed
				print("ROVER: Decreasing speed")
				speed = speed - 0.1
				if speed < 0.2:
					print("ROVER: speed is less than 20% Duty Cycle, speed up!")
					speed = 0.2
				print(f"speed : {speed * 100}%\n")

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
						shoulder_up(speed)	
					elif value < -0.3:
						print("ROVER: Shoulder goes down")
						shoulder_down(speed)
					else:
						print("ROVER: Shoulder stays in motion")
						shoulder_off()
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 2: # 2 is for elbow
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Elbow goes up")
						elbow_up(speed) 
					elif value > 0.3:
						print("ROVER: Elbow goes down")
						elbow_down(speed) 
					else:
						print("ROVER: Elbow stays in motion")
						elbow_off() 
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 0: # 0 is for base
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Base goes up")
						base_up(speed)	
					elif value > 0.3:
						print("ROVER: Base goes down")
						base_down(speed)
					else:
						print("ROVER: Base stays in motion")
						base_off() 
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 3: # 3 is for wrist main
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist goes up")
						wrist_up(speed) 
					elif value > 0.3:
						print("ROVER: Wrist goes down")
						wrist_down(speed) 
					else:
						print("ROVER: Wrist stays in motion")
						wrist_off() 
					armControlerLastMovingJoint = movingJoint
					continue

			elif movingJoint == 4: # 4 is for wrist12
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist 1 and 2 goes up")
						wrist12_up(speed) 	
					elif value > 0.3:
						print("ROVER: Wrist 1 and 2 goes down")
						wrist12_down(speed) 
					else:
						print("ROVER: Wrist 1 and 2 stays in motion")
						wrist12_off() 
					armControlerLastMovingJoint = movingJoint
					continue
				if axis == 2: # Y-axis
					if value < -0.3:
						print("ROVER: Wrist 1 and 2 goes left")
						wrist12_left(speed) 	
					elif value > 0.3:
						print("ROVER: Wrist 1 and 2 goes right")
						wrist12_right(speed) 
					else:
						print("ROVER: Wrist 1 and 2 stays in motion")
						wrist12_off()
					armControlerLastMovingJoint = movingJoint
					continue
			elif movingJoint == 5: # 5 is for claw
				if axis == 3: # Y-axis
					if value < -0.3:
						print("ROVER: claw opens")
						claw_open(speed) 
					elif value > 0.3:
						print("ROVER: claw closes")
						claw_close(speed) 
					else:
						print("ROVER: claw stays in motion")
						claw_off()
					armControlerLastMovingJoint = movingJoint
					continue
			
			elif movingJoint == 6: # 6 is for drive
				if axis == 0:  # X-axis (left-right)
					if value < -0.3:
						print("ROVER: Drive left")
						drive_left(speed)
					elif value > 0.3:
						print("ROVER: Drive right")
						drive_right(speed)
					else:
						print("ROVER: Drive stop")
						drive_stop()
					driveControlerLastMovingJoint = movingJoint
					continue
				
				elif axis == 1:  # Y-axis (up-down)
					if value < -0.3:
						print("ROVER: Drive forward")
						drive_forward(speed)
					elif value > 0.3:
						print("ROVER: Drive backward")
						drive_backward(speed)
					else:
						print("ROVER: Drive stop")
						drive_stop()
					driveControlerLastMovingJoint = movingJoint
					continue
		# elif command == "hat_motion":
		# 	hat = int(parts[1])
		# 	value = float(parts[2])
		# 	print("ROVER: Hat Motion:", hat, value)
'''

