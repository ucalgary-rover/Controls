import asyncio
import websockets
import pygame
import json

# WebSocket server address
uri = "ws://192.168.1.22:12345" ############################################### ws://roverNUC_ip:some random port

# init variables
i = 0
# Keep Drive as last value in list
jointMovement = ['base', 'shoulder', 'elbow', 'wrist main', 'wrist12', 'claw', 'drive']
# the -1 makes it so that the shuffle excludes the drive mode, which will be 
# explicitly set when the input comes from controler 0
jointLength = len(jointMovement) - 1

# set the last moving joint to be an invalide one, so all three of the data will be re written right away
lastMovingJoint = -1
lastAxis = 0
lastValue = 0

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
	async with websockets.connect(uri) as websocket:
		try:
			while True:
				global i
				global movingJoint

				global lastMovingJoint
				global lastAxis
				global lastValue

				#event processing step.
				for event in pygame.event.get():
		            #to exit
					if event.type == pygame.QUIT:
						pygame.quit()
						exit()

		            #button on the joystick is pressed down
					if event.type == pygame.JOYBUTTONDOWN:
						if event.button == 2:
							print("BASE: off")

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

						elif event.button == 9: # and event.instance_id == 1:
							print("BASE: Next Joint Movement mode")
							# Next mode
							i = (i + 1)%jointLength
							print(jointMovement[i])
						
						#what gets sent to the rover nuc
						# command[0] = 0 is for button down
						command = [0,event.button,movingJoint]
						await websocket.send(json.dumps(command))
						print("BASE: Button Pressed:", event.button, movingJoint)
							
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
						# button on the joystick is pressed up
						if event.type == pygame.JOYBUTTONUP:
							# command[0] = 1 is for button up
							command = [1,event.button,0,movingJoint]
							await websocket.send(json.dumps(command))
							#if event.button == 2:
								#print("BASE: off")
								#exit()

						if event.type == pygame.JOYAXISMOTION and event.axis != None and event.value != None:
							if (movingJoint == lastMovingJoint) and (event.axis == lastAxis) and ((event.value > 0.3 and lastValue > 0.3) or (event.value < -0.3 and lastValue < -0.3) or ((0.3 > event.value > -0.3) and (0.3 > lastValue > -0.3))):
								continue
							else:
								# command[0] = 2 is for axis_motion
								command = [2,event.axis,event.value,movingJoint]

								await websocket.send(json.dumps(command))
								print("BASE: Axis Motion:", event.axis, event.value, movingJoint)
								lastMovingJoint = movingJoint
								lastValue = event.value
								lastAxis = event.axis

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
