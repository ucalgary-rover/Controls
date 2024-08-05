import pygame

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
		if event.type == pygame.JOYAXISMOTION:
			print("Axis Motion:", event.axis, event.value)
		if event.type == pygame.JOYHATMOTION:
			print("Hat Motion:", event.hat, event.value)

print("I'm done")

