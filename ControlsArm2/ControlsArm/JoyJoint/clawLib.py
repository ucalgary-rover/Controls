from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
import traceback
import time

'''
Claw lib for initalization and movement:

function names:
claw_init - initalizes claw 
claw_open - opens claw
claw_close - closes claw
claw_off - claw stays in motion 

'''
try: 
    VHubSerial_motors = 697103
    VHubSerial_servo = 697066

    smoothing = 0.005

    claw = RCServo()

    def connect_motor(motor):
	    try:
		    motor.openWaitForAttachment(5000)
		    print("Test connection")
	    except:
		    print("Failed to connect")

    isClawAttached = False
    clawSpeeds = [100, 80]
    isClawMoving = False

    stopFlag = False

    isClawMoving = False 


    def onAttach_motor(self):
        print(" {0} attached!".format(self.getHubPort()))

    def onDetach(self):
        print("A motor detached!")

    def onError(self,code, description):
	    print("Code: " + ErrorEventCode.getName(code))
	    print("Description: " + str(description))
	    print("----------")

    def claw_init():
	    global claw
	    try:
		    # Functions to initialize components 
		    print(f"\nInitializing...\n\n") 
		    #initialize_motors() 
		    print(f"\nSuccessfully initialized!\n")
	
		
		    claw.setChannel(0)
		    claw.setHubPort(0)
		    claw.setDeviceSerialNumber(VHubSerial_servo)
		    claw.openWaitForAttachment(1000)
		    claw.setVoltage(RCServoVoltage.RCSERVO_VOLTAGE_7_4V)
		    claw.setMinPulseWidth(500)
		    claw.setMaxPulseWidth(2500)		


		    # Main loop of code, stopFlag becomes True when 'p' is pressed 
                    while(stopFlag == False): 
                            time.sleep(0.1)
                            #baseMotor.setVelocityLimit(motorsInfo[baseIndex][5]) 
            except PhidgetException as ex: 
		    traceback.print_exc() 
		    print() 
		    print("PhidgetException " + str(ex.code) + " (" + ex.description + "): " + ex.details)
		    print(f"Successfully quit program.\n\nGoodbye!\n")



    def claw_open():
	    global claw
	    try:
		    isClawMoving = True
		    claw.setTargetPosition(clawSpeeds[0]) 
		    claw.setEngaged(True)
	    except PhidgetException as ex:
		    print(f"PhidgetException in claw_open: {ex.code} ({ex.description})")

    def claw_close():
	    global claw
	    try: 
		    isClawMoving = True
		    claw.setTargetPosition(clawSpeeds[1]) 
		    claw.setEngaged(True) 
	    except PhidgetException as ex:
		    print(f"PhidgetException in claw_close: {ex.code} ({ex.description})")
	
    def claw_off():
	    global claw
	    try:
		    if isClawMoving:
			    claw.setTargetPosition(90)
			    claw.setEngaged(False)
			    isClawMoving = False
	    except PhidgetException as ex:
		    print(f"PhidgetException in claw_off: {ex.code} ({ex.description})")

except Exception as ex:
        print(f"An exception occurred: {ex}")		









