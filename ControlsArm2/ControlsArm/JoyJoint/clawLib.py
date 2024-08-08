from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
import traceback
import time

'''
Claw lib for initalization and movement:

function names:
claw_init - initalizes claw & also science tool(assuming it works)
claw_open - opens claw
claw_close - closes claw
claw_off - claw stays in motion 

tool_right - science tool servo turns right - 0 degrees
tool_middle - science tool servo goes middle - 90 degrees
tool_left - science tool servo turns left - 180 degrees
'''
try: 
	VHubSerial_motors = 697103
	VHubSerial_servo = 697066

	smoothing = 0.005

	claw = RCServo()
	'''tool = RCServo()'''

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
############################################# added these for the science tool
	'''
	isToolAttached = False
	toolPositions = [0, 90, 180]
	isToolMoving = False
	'''
	

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


			'''## both channel and Hubport will depend on what port we physcially connect the sci-tool to on the controller phidget & vint hub
			# other libs dont set channel so might not be neccessary because i think it defaults to channel 0 anyways
			tool.setChannel(0)
			tool.setHubPort(1)
			##### this may or may not need to be changed based off the serial code on the acc servo or vint hub that its connected to
			tool.setDeviceSerialNumber(VHubSerial_servo)
			tool.openWaitForAttachment(1000)
			## this stands assuming we running the servos on 7.4v
			tool.setVoltage(RCServoVoltage.RCSERVO_VOLTAGE_7_4V)
			## pre sure these are same as for the claw if they are the same servo(part defined value)
			tool.setMinPulseWidth(500)
			tool.setMaxPulseWidth(2500)'''	


		except PhidgetException as ex: 
			traceback.print_exc() 
			print() 
			print("PhidgetException " + str(ex.code) + " (" + ex.description + "): " + ex.details)
			print(f"Successfully quit program.\n\nGoodbye!\n")

	def claw_open():
		global claw
		global isClawMoving
		try:
			isClawMoving = True
			claw.setTargetPosition(clawSpeeds[0]) 
			claw.setEngaged(True)
		except PhidgetException as ex:
			print(f"PhidgetException in claw_open: {ex.code} ({ex.description})")

	def claw_close():
		global claw
		global isClawMoving
		try: 
			isClawMoving = True
			claw.setTargetPosition(clawSpeeds[1]) 
			claw.setEngaged(True) 
		except PhidgetException as ex:
			print(f"PhidgetException in claw_close: {ex.code} ({ex.description})")
	
	def claw_off():
		global claw
		global isClawMoving
		try:
			if isClawMoving:
				claw.setTargetPosition(90)
				claw.setEngaged(False)
				isClawMoving = False
		except PhidgetException as ex:
			print(f"PhidgetException in claw_off: {ex.code} ({ex.description})")


	'''
	def tool_right():
		global tool
  		global isToolMoving
		try:
			isToolMoving = True
			tool.setTargetPosition(toolPositions[0]) # 0 degrees
			tool.setEngaged(True)
		except PhidgetException as ex:
			print(f"PhidgetException in tool_right: {ex.code} ({ex.description})")

	def tool_middle():
		global tool
  		global isToolMoving
		try:
			isToolMoving = True
			tool.setTargetPosition(toolPositions[1]) # 90 degrees
			tool.setEngaged(True)
		except PhidgetException as ex:
			print(f"PhidgetException in tool_middle: {ex.code} ({ex.description})")
	
	def tool_right():
		global tool
		global isToolMoving
		try:
			isToolMoving = True
			tool.setTargetPosition(toolPositions[2]) # 180 degrees
			tool.setEngaged(True)
		except PhidgetException as ex:
			print(f"PhidgetException in tool_left: {ex.code} ({ex.description})")
'''


except Exception as ex:
	print(f"An exception occurred: {ex}")		


