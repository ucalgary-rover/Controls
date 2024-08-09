import asyncio
import websockets
import pygame

# WebSocket server address
uri = "ws://192.168.1.22:12345" ############################################### ws://roverNUC_ip:some random port

# init variables
i = 0
# Keep Drive as last value in list
jointMovement = ['base', 'shoulder', 'elbow', 'wrist main', 'wrist12', 'claw', 'drive']
#jointMovementdrive = [ 'drive', 'tool_left', 'tool_middle', 'tool_right' ]
# the -1 makes it so that the shuffle excludes the drive mode, which will be 
# explicitly set when the input comes from controler 0
jointLength = len(jointMovement) - 1
#jointLengthdrive = len(jointMovementdrive) - 1

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
#movingJointdrive = 0



print("BASE: Initialized joystick ", joystick0.get_name())
print("BASE: Initialized joystick ", joystick1.get_name())

async def send_commands():
	async with websockets.connect(uri) as websocket:
		try:
			while True:
				global i
				global movingJoint
				#global j
				#global movingJointdrive
				#event processing step.
				for event in pygame.event.get():
		            #to exit
					if event.type == pygame.QUIT:
						pygame.quit()
						exit()

		            #button on the joystick is pressed down
					if event.type == pygame.JOYBUTTONDOWN:
						if event.button == 2:
							print("BASE: bristles on")

						elif event.button == 5:
							print("BASE: Increase driver speed")

						elif event.button == 4:
							print("BASE: Decrease driver speed")

						# to make sure only remote #1 can cycle between arm joints, added event.instance_id == 1
						elif event.button == 8: # and event.instance_id == 1:
							print("BASE: Previous Joint Movement mode")
							# Next mode
							i = (i - 1)%jointLength
							print(jointMovement[i])
							#j = (j - 1)%jointLengthdrive
							#print(jointMovementdrive[j])

						elif event.button == 9: # and event.instance_id == 1:
							print("BASE: Next Joint Movement mode")
							# Next mode
							i = (i + 1)%jointLength
							print(jointMovement[i])
							#j = (j + 1)%jointLengthdrive
							#print(jointMovementdrive[j])
						
						#what gets sent to the rover nuc
						command = f"button_down:{event.button}::{movingJoint}"
						#command = f"button_down:{event.button}::{movingJointdrive}:{movingJoint}"
						await websocket.send(command)
						print("BASE: Button Pressed:", event.button, movingJoint)
						#print("BASE: Button Pressed:", event.button, movingJointdrive, movingJoint)
							
					print(event.__dict__)
					# check if its the drive controler or the arm contoler
					if event.__dict__.get("instance_id") != None:
						if event.__dict__["instance_id"] == 0:
							# drive co.ntroler:
							movingJoint = jointLength
							#movingJointdrive = j
						  
						elif event.__dict__["instance_id"]  == 1: 
							# arm controler
							movingJoint = i
						
						else:
							# for future if we want to add extra controlers
							pass
						print(jointMovement[movingJoint])
						#print(f"Drive: {jointMovementdrive[movingJointdrive]}")
						# button on the joystick is pressed up
						if event.type == pygame.JOYBUTTONUP:
							command = f"button_up:{event.button}::{movingJoint}"
							#command = f"button_up:{event.button}::{movingJointdrive}:{movingJoint}"
							await websocket.send(command)
							#if event.button == 2:
								#print("BASE: bristles on")
				

						if event.type == pygame.JOYAXISMOTION and event.axis != None and event.value != None:
							command = f"axis_motion:{event.axis}:{event.value}:{movingJoint}"
							#command = f"axis_motion:{event.axis}:{event.value}:{movingJointdrive}:{movingJoint}"
							await websocket.send(command)
							print("BASE: Axis Motion:", event.axis, event.value, movingJoint)
							#print("BASE: Axis Motion:", event.axis, event.value, movingJointdrive, movingJoint)

						#if event.type == pygame.JOYHATMOTION:
						#	command = f"hat_motion:{event.hat}:{event.value}:{movingJoint}"
						#	await websocket.send(command)
						#	print("BASE: Hat Motion:", event.hat, event.value, movingJoint)
		                
		except ( websockets.InvalidHandshake) as e:
			print(f"InvalidHandshake exception error: {e}")
			#await asyncio.sleep(5)
			exit()
		except (websockets.InvalidURI) as e:
			print(f"InvalidURI exception error: {e}")
			#await asyncio.sleep(5)
			exit()
		except (websockets.ConnectionClosedError) as e:
			print(f"ConnectionClosedError exception error: {e}")
			#await asyncio.sleep(5)
			print("No connection! Exiting...")
			exit()
		except Exception as e:
			print(f"unexpected error: {e}")
			#await asyncio.sleep(5)
			exit()

asyncio.get_event_loop().run_until_complete(send_commands())

