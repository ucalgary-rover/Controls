import asyncio
import websockets
import pygame

# WebSocket server address
uri = "ws://192.168.1.22:12345" ############################################### ws://roverNUC_ip:some random port

# Initialize Pygame and joystick
pygame.init()
pygame.joystick.init()

joystick_count = pygame.joystick.get_count()
print("Number of joysticks: ", joystick_count)
joystick0 = pygame.joystick.Joystick(0)
#joystick1 = pygame.joystick.Joystick(1)
joystick0.init()
#joystick1.init()

print("BASE: Initialized joystick ", joystick0.get_name())
#print("BASE: Initialized joystick ", joystick1.get_name())

async def send_commands():
    async with websockets.connect(uri) as websocket:
        while True:
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

                    elif event.button == 6:
                        print("BASE: Drive mode")

                    elif event.button == 8:
                        print("BASE: Previous Joint Movement mode")

                    elif event.button == 9:
                        print("BASE: Next Joint Movement mode")
                
                # button on the joystick is pressed up
                if event.type == pygame.JOYBUTTONUP:
                    command = f"button_up:{event.button}"
                    await websocket.send(command)
                    if event.button == 2:
                        print("BASE: off")
                        exit()

                if event.type == pygame.JOYAXISMOTION:
                    command = f"axis_motion:{event.axis}:{event.value}"
                    await websocket.send(command)
                    print("BASE: Axis Motion:", event.axis, event.value)

                if event.type == pygame.JOYHATMOTION:
                    command = f"hat_motion:{event.hat}:{event.value}"
                    await websocket.send(command)
                    print("BASE: Hat Motion:", event.hat, event.value)

asyncio.get_event_loop().run_until_complete(send_commands())
