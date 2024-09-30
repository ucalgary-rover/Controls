# Joshua Liu
# 
# Possible keys for controls: a-z, 0-9
# 
# VINT Hub 1 wiring (6 Steppers)
#   Port 0: Base
#   Port 1: Shoulder
#   Port 2: Elbow
#   Port 3: Wrist
#   Port 4: Wrist1
#   Port 5: Wrist2
# Wiring the Claw
#   VINT Hub 2 Port 0
#   Servo Controller Port 0
#   This file also includes initializing the drive motors


from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
from Phidget22.Devices.DCMotor import *
from pynput import keyboard
from pynput.keyboard import Key, Listener
import traceback
import time
import pygame

shoulderIndex = 1
shoulderControls = [(0,1), (0,-1)] # change this to change the controls of the Shoulder motor


stopFlag = False

VHubSerial_motors = 697103
VHubSerial_servo = 697066

smoothing = 0.005 # Time delay (in seconds) for a motor to change from Attached to stopped
motorsAttached = [False, False, False, False, False, False]
encoders = []



def connect_motor(motor):
    try:
        motor.openWaitForAttachment(5000)
    except:
        print("Failed to connect.")

def init_motor(motor, currentLimit):
    if motor.getAttached():
        motor.setDeviceSerialNumber(697178)
        motor.setCurrentLimit(currentLimit)
        motor.setTargetVelocity(0)
        motor.setAcceleration(5)
        motor.setFanMode(FanMode.FAN_MODE_ON)#FAN_MODE_AUTO


def on_press(key):
    global isClawMoving
    print("KEY")
    try: 
        if key.char == shoulderControls[0] and motorsAttached[shoulderIndex] == True: 
            if not shoulderMotor.getIsMoving(): 
                shoulderMotor.setVelocityLimit(motorsInfo[shoulderIndex][5]) 
        elif key.char == shoulderControls[1] and motorsAttached[shoulderIndex] == True: 
            if not shoulderMotor.getIsMoving(): 
                shoulderMotor.setVelocityLimit(-motorsInfo[shoulderIndex][5]) 
    except AttributeError: 
        print("Special key {0} pressed".format(key))
def on_release(key): 
    global shoulderMotor
    try: 
        if shoulderControls[0] == key.char or shoulderControls[1] == key.char: 
            lim = shoulderMotor.getVelocityLimit() 
            shoulderMotor.setVelocityLimit(lim * 3 / 4) 
            time.sleep(smoothing / 4) 
            shoulderMotor.setVelocityLimit(lim / 2) 
            time.sleep(smoothing / 4) 
            shoulderMotor.setVelocityLimit(lim / 4) 
            time.sleep(smoothing / 4) 
            shoulderMotor.setVelocityLimit(0)
        if key.char == 'p':
            print("p is pressed")
            print(f"\nQuitting program...")
            for i in range(len(motors)): 
                if(motors[i].getAttached() == True): 
                    motors[i].setEngaged(False) 
                    motors[i].close() 
            stopFlag = True
    except AttributeError: 
        print("Special key {0} released".format(key)) 

def onAttach_motor(self): 
    print(" {0} attached!".format(self.getHubPort())) 
    motorsAttached[self.getHubPort()] = True

def onAttach_encoder(self): 
    print("Encoder {0} attached!".format(self.getHubPort())) 

def onDetach(self): 
    print("A motor detached!")

def onError(self,code, description): 
    print("Code: " + ErrorEventCode.getName(code)) 
    print("Description: " + str(description)) 
    print("----------")
    
def initialize_motors(): 
    global motors, motorsInfo
    for i in range(len(motors)): 
        motors[i].setDeviceSerialNumber(VHubSerial_motors) 
        motors[i].setHubPort(i) 
        motors[i].setOnAttachHandler(onAttach_motor) 
        motors[i].setOnDetachHandler(onDetach) 
        motors[i].setOnErrorHandler(onError) 
        try: 
            motors[i].openWaitForAttachment(1000) # If having motor connection timout issues, increase this number 
        except: 
            print(" " + str(i) + " not attached")
        if (motors[i].getAttached() == True): 
            motors[i].setControlMode(StepperControlMode.CONTROL_MODE_RUN) 
            motors[i].setCurrentLimit(motorsInfo[i][0]) 
            motors[i].setHoldingCurrentLimit(motorsInfo[i][1])
            motors[i].setRescaleFactor((1/16) * 1.8 * (1/motorsInfo[i][2]) * (1/motorsInfo[i][3])) # (1/16) * Step angle * (1/Gearbox ratio) * (1/Gear ratio) p
            motors[i].setAcceleration(motorsInfo[i][4])
            motors[i].setVelocityLimit(0) 
            motors[i].setEngaged(True) 
            motors[i].setDataInterval(motors[i].getMinDataInterval())
            
def main(): 
    # All motors, and positions are declared as global variables 
    global motors, motorsInfo, shoulderMotor
    global shoulderInitialPos
    
    
    shoulderMotor = Stepper() 
    shoulderInfo = [3, 1.68, 100, 1, 5, 5]
    shoulderInitialPos = 0
    
    motors = [shoulderMotor] 
    motorsInfo = [shoulderInfo] 

    # Functions to initialize components 
    print(f"\nInitializing...\n\n") 
    #initialize_motors() 
    print(f"\nSuccessfully initialized!\n")
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
                if event.value == shoulderControls[0] and motorsAttached[shoulderIndex] == True: 
                    if not shoulderMotor.getIsMoving(): 
                        shoulderMotor.setVelocityLimit(motorsInfo[shoulderIndex][5])
                elif event.value == shoulderControls[1] and motorsAttached[shoulderIndex] == True: 
                    if not shoulderMotor.getIsMoving(): 
                        shoulderMotor.setVelocityLimit(-motorsInfo[shoulderIndex][5]) 

    print("I'm done")
    """
    claw = RCServo()
    claw.setChannel(0)
    claw.setHubPort(0)
    claw.setDeviceSerialNumber(VHubSerial_servo)
    claw.openWaitForAttachment(1000)
    claw.setVoltage(RCServoVoltage.RCSERVO_VOLTAGE_7_4V)
    claw.setMinPulseWidth(500)
    claw.setMaxPulseWidth(2500)
    """

    # Main loop of code, stopFlag becomes True when 'p' is pressed 
    while(stopFlag == False): 
        time.sleep(0.1)
    #baseMotor.setVelocityLimit(motorsInfo[baseIndex][5]) 


if __name__ == "__main__": 
    main()

