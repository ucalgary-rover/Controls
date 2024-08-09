import asyncio
import websockets
import pygame
import msgpack

# --------------------------------------------- #
# ################# IMPORTANT: ##################
# 
# If using the claw, set usingSciTools to False only
# if using science tools, set usingSciTools to TRUE
# --------------------------------------------- #
usingSciTools = False

# WebSocket server address
uri = "ws://192.168.1.22:12345" ############################################### ws://roverNUC_ip:some random port

# init variables
i = 0
# Keep Drive as last value in the following list, no matter the definition
if not usingSciTools:
	jointMovement = ['base', 'shoulder', 'elbow', 'wrist main', 'wrist12', 'claw', 'drive']
else:
	jointMovement = [ 'tool_left', 'tool_middle', 'tool_right', 'drive']
# the -1 makes it so that the shuffle excludes the drive mode, which will be 
# explicitly set when the input comes from controler 0
jointLength = len(jointMovement) - 1

# set the last moving joint to be an invalide one, so all three of the data will be re written right away
lastDriveMovingJoint = -1
lastDriveAxis = 0
lastDriveValue = 0
lastArmMovingJoint = -1
lastArmAxis = 0
lastArmValue = 0

# Initialize Pygame and joystick
pygame.init()
pygame.joystick.init()

joystick_count = pygame.joystick.get_count()
print("Number of joysticks: ", joystick_count)
joystick0 = pygame.joystick.Joystick(0)
joystick1 = pygame.joystick.Joystick(1)
joystick0.init()
joystick1.init()
movingJoint = 0



print("BASE: Initialized joystick ", joystick0.get_name())
print("BASE: Initialized joystick ", joystick1.get_name())

async def send_commands():
	global i
	global movingJoint
	global usingSciTools

	global lastDriveMovingJoint
	global lastDriveAxis
	global lastDriveValue
	global lastArmMovingJoint
	global lastArmAxis
	global lastArmValue
	async with websockets.connect(uri) as websocket:
		try:
			while True:
				#event processing step.
				for event in pygame.event.get():
					if(event.type == pygame.JOYAXISMOTION):
						if ((event.__dict__["instance_id"]  == 0) and (movingJoint == lastDriveMovingJoint) and (event.axis == lastDriveAxis) and ((event.value > 0.3 and lastDriveValue > 0.3) or (event.value < -0.3 and lastDriveValue < -0.3) or ((0.3 > event.value > -0.3) and (0.3 > lastDriveValue > -0.3)))):
							continue
						if ((event.__dict__["instance_id"]  == 1) and (movingJoint == lastArmMovingJoint) and (event.axis == lastArmAxis) and ((event.value > 0.3 and lastArmValue > 0.3) or (event.value < -0.3 and lastArmValue < -0.3) or ((0.3 > event.value > -0.3) and (0.3 > lastArmValue > -0.3)))):
							continue
		            #to exit
					if event.type == pygame.QUIT:
						pygame.quit()
						exit()

		            #button on the joystick is pressed down
					if event.type == pygame.JOYBUTTONDOWN:
						if event.button == 2:
							if usingSciTools:
								print("BASE: bristles on")
							else:
								continue

						elif event.button == 5:
							print("BASE: Increase driver speed")

						elif event.button == 4:
							print("BASE: Decrease driver speed")

						# to make sure only remote #1 can cycle between arm joints, added event.instance_id == 1
						elif event.button == 8: # and event.instance_id == 1:
							if not usingSciTools:
								print("BASE: Previous Joint Movement mode")
							else:
								print("BASE: Previous Tool mode")
							# Next mode
							i = (i - 1)%jointLength
							print(f"Now on {jointMovement[i]}")

						elif event.button == 9: # and event.instance_id == 1:
							print("BASE: Next Joint Movement mode")
							# Next mode
							i = (i + 1)%jointLength
							print(jointMovement[i])
						if 	event.button in [2,4,5]	:		
							#what gets sent to the rover nuc
							# command[0] = 0 is for button down
							command = [0, event.button, int(event.__dict__["instance_id"])]
							packedCommand = msgpack.packb(command)
							await websocket.send(packedCommand)
							print("BASE: Button Pressed:", event.button, int(event.__dict__["instance_id"]))
						continue
							
					print(event.__dict__)
					# check if its the drive controler or the arm contoler
					if event.__dict__.get("instance_id") != None:
						if event.__dict__["instance_id"] == 0:
							# drive co.ntroler:
							movingJoint = jointLength
						  
						elif event.__dict__["instance_id"]  == 1: 
							# arm controler
							movingJoint = i
						
						else:
							# for future if we want to add extra controlers
							pass
						print(jointMovement[movingJoint])

						if event.type == pygame.JOYAXISMOTION and event.axis != None and event.value != None:
							# command[0] = 2 is for axis_motion
							command = [2,event.axis,event.value,movingJoint]
							packedCommand = msgpack.packb(command)
							await websocket.send(packedCommand)
							print("BASE: Axis Motion:", event.axis, event.value, movingJoint)
							if event.__dict__["instance_id"] == 0:
								lastDriveMovingJoint = movingJoint
								lastDriveValue = event.value
								lastDriveAxis = event.axis
							elif event.__dict__["instance_id"]  == 1: 
								lastArmMovingJoint = movingJoint
								lastArmValue = event.value
								lastArmAxis = event.axis

						#if event.type == pygame.JOYHATMOTION:
						#	command = f"hat_motion:{event.hat}:{event.value}:{movingJoint}"
						#	await websocket.send(command)
						#	print("BASE: Hat Motion:", event.hat, event.value, movingJoint)
		                
		except ( websockets.InvalidHandshake) as e:
			print(f"InvalidHandshake exception error: {e}")
			await asyncio.sleep(5)
		except (websockets.InvalidURI) as e:
			print(f"InvalidURI exception error: {e}")
			await asyncio.sleep(5)
		except (websockets.ConnectionClosedError) as e:
			print(f"ConnectionClosedError exception error: {e}")
			await asyncio.sleep(5)
		except Exception as e:
			print(f"unexpected error: {e}")
			await asyncio.sleep(5)

asyncio.get_event_loop().run_until_complete(send_commands())
