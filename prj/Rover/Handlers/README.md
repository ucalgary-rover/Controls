General:
- Assumes degrees measured counterclockwise from positive y-axis (on the
joystick) 

- Assumes positive y-axis moves the rover foward when strafing and
radial turning (based on the rover's front)

- Assumes wheel stepper motors angle measured counterclockwise from positive y-axis (on the
joystick) 

- To prevent undesired large turns, angles provided to the stepper motors are restricted to a 90 - -90 range
- Angles must be adjusted to fit within this range before passing them into the steppers
- Large changes in wheel angle will cause the wheels to stop spinning


Terminology:
- "turning" is used to refer to the steppers pointing the wheels in a different direction 
- "spinning" is used to refer to the dc motors rotating the wheels forward or backwards

-------------> Spot turning: Causes the rover to spin around on the spot
- Uses the angle from the right joystick to determine the speed of the turn (based on distance from 0 degrees)
- Moving the joystick to the left results in a ccw turn while a cw turn occurs when pointed to the right
- The angle at which the maximum speed is reached is adjustable

Process:
- Compares the joystick angle to determine the speed
- Calculates wheel angles from the dimensions of the rover
- Turns and spins wheels 


-------------> Radial turning: Causes the rover to move and turn like a car (only front wheels turn)
- Uses the angle from the right joystick to determine the sharpness of the turn (based on distance from the vertical axis and mirrored on the bottom hemisphere of the joystick)
- Ex. The left quadrant of the upper hemisphere turns in the same direction as the right quadrant of the bottom hemisphere
- Uses the magnitude of the left joystick to determine speed (based on distance from center along the vertical axis)
- Can only be used when the rover is moving forward or backward

Process:
- Determines the heading angle from the right joystick's distance from the vertical axis (in radians)
**For edge cases on the horizontal axis, the rover turns in the previous turning direction
- Puts the heading angle into ackerman steering calculations to determine individual wheel angles (in degrees)
- Turns and spins the wheels

-------------> Strafing: Causes the rover to move directly in the desired direction without turning
- Uses the angle of the left joystick to determine the direction to move in
**If movement backward is desired, the angle is mirrored so that it ends up in the acceptable range and the wheels spin backwards
***For edge cases at 90 and -90, whichever is the closest is chosen as the target. The wheels spin forward or backward depending on desired direction of movement
- Uses the magnitude of the left joystick to determine speed (distance from center)

Process:
- Uses the angle of the left joystick to determine the direction to move in
- Calculates for any alternate cases
- Uses the magnitude of the left joystick to determine speed
- Turns and spins the wheels