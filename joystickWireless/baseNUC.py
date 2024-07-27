import asyncio
import websockets
import pygame

# WebSocket server address
uri = "ws://192.168.1.22:12345" ############################################### ws://roverNUC_ip:some random port

# init variables
i = 0
# Keep Drive as last value in list
jointMovement = ['shoulder', 'elbow', 'base', 'wrist main', 'wrist12', 'claw', 'drive']
# the -1 makes it so that the shuffle excludes the drive mode, which will be 
# explicitly set when the input comes from controler 0
jointLength = len(jointMovement) - 1

# Initialize Pygame and joystick
pygame.init()
pygame.joystick.init()

joystick_count = pygame.joystick.get_count()
print("Number of joysticks: ", joystick_count)
joystick0 = pygame.joystick.Joystick(0)
joystick1 = pygame.joystick.Joystick(1)
joystick0.init()
joystick1.init()



print("BASE: Initialized joystick ", joystick0.get_name())
print("BASE: Initialized joystick ", joystick1.get_name())

async def send_commands():
	async with websockets.connect(uri) as websocket:
		try:
			while True:
				global i
				#event processing step.
				for event in pygame.event.get():
		            #to exit
					if event.type == pygame.QUIT:
						pygame.quit()
						exit()

		            #button on the joystick is pressed down
					if event.type == pygame.JOYBUTTONDOWN:
						#what gets sent to the rover nuc
						command = f"button_down:{event.button}"
						await websocket.send(command)
						print("BASE: Button Pressed:", event.button)

						if event.button == 2:
							print("BASE: off")

						elif event.button == 3:
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
					print(event.__dict__)
					# check if its the drive controler or the arm contoler
					if event.__dict__.get("instance_id") != None:
						if event.__dict__["instance_id"] == 0:
							print("after if 0")
							# drive co.ntroler:
							movingJoint = jointLength
						  
						elif event.__dict__["instance_id"]  == 1: 
							# arm controler
							print("after if 1")
							movingJoint = i
						
						else:
							# for future if we want to add extra controlers
							pass
						print(jointMovement[movingJoint])
						# button on the joystick is pressed up
						if event.type == pygame.JOYBUTTONUP:
							command = f"button_up:{event.button}::{movingJoint}"
							await websocket.send(command)
							if event.button == 2:
								print("BASE: off")
								exit()

						if event.type == pygame.JOYAXISMOTION:
							command = f"axis_motion:{event.axis}:{event.value}:{movingJoint}"
							await websocket.send(command)
							print("BASE: Axis Motion:", event.axis, event.value, movingJoint)

						if event.type == pygame.JOYHATMOTION:
							command = f"hat_motion:{event.hat}:{event.value}:{movingJoint}"
							await websocket.send(command)
							print("BASE: Hat Motion:", event.hat, event.value, movingJoint)
		                
		except (websockets.ConnectionClosedError, websockets.InvalidURI, websockets.InvalidHandshake) as e:
			print(f"exception error: {e}")
			await asyncio.sleep(5)
		except Exception as e:
			print(f"unexpected error: {e}")
			await asyncio.sleep(5)

asyncio.get_event_loop().run_until_complete(send_commands())
