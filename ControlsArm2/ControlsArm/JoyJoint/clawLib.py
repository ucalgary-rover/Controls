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
	tool_right = RCServo()
	tool_middle = RCServo()
	tool_left = RCServo()
	tools = {"right" : tool_right, "middle" : tool_middle, "left" : tool_left}
	toolsMoving = {"right" : False, "middle" : False, "left" : False}

	def connect_motor(motor):
		try:
			motor.openWaitForAttachment(5000)
			print("Test connection")
		except:
			print("Failed to connect")

	# isClawAttached = False  // unused
	speeds = [100, 80]
	isClawMoving = False

	# stopFlag = False  //unused
	

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
		
			claw.setChannel(0)
			claw.setHubPort(0)
			claw.setDeviceSerialNumber(VHubSerial_servo)
			claw.openWaitForAttachment(1000)
			claw.setVoltage(RCServoVoltage.RCSERVO_VOLTAGE_7_4V)
			claw.setMinPulseWidth(500)
			claw.setMaxPulseWidth(2500)		

			print(f"\nSuccessfully initialized!\n")	


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
			claw.setTargetPosition(speeds[0]) 
			claw.setEngaged(True)
		except PhidgetException as ex:
			print(f"PhidgetException in claw_open: {ex.code} ({ex.description})")

	def claw_close():
		global claw
		global isClawMoving
		try: 
			isClawMoving = True
			claw.setTargetPosition(speeds[1]) 
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


	def tool_init(str):
		global tools
		try:
			i=0
			for tool in tools.values():
		    # Functions to initialize components 
				print(f"\nInitializing tool{i}...\n\n") 	
			
				tool.setChannel(i)
				tool.setHubPort(0)
				tool.setDeviceSerialNumber(VHubSerial_servo)
				tool.openWaitForAttachment(1000)
				tool.setVoltage(RCServoVoltage.RCSERVO_VOLTAGE_7_4V)
				tool.setMinPulseWidth(500)
				tool.setMaxPulseWidth(2500)		

				print(f"\nSuccessfully initialized tool {i}!\n")	

		except PhidgetException as ex: 
			traceback.print_exc() 
			print() 
			print("PhidgetException " + str(ex.code) + " (" + ex.description + "): " + ex.details)
			print(f"Successfully quit program.\n\nGoodbye!\n")

	def tool_lower(str):
		if (str not in ["right", "middle", "left"]):
			print("Invalide Tool")
			exit()
		global tools
		global toolsMoving
		try:
			toolsMoving[str] = True
			tools[str].setTargetPosition(speeds[0]) 
			tools[str].setEngaged(True)
		except PhidgetException as ex:
			print(f"PhidgetException in tool_open for {str}: {ex.code} ({ex.description})")

	def tool_raise(str):
		if (str not in ["right", "middle", "left"]):
			print("Invalide Tool")
			exit()
		global tools
		global toolsMoving
		try: 
			toolsMoving[str] = True
			tools[str].setTargetPosition(speeds[1]) 
			tools[str].setEngaged(True) 
		except PhidgetException as ex:
			print(f"PhidgetException in tool_close for {str}: {ex.code} ({ex.description})")
	
	def tool_off(str):
		if (str not in ["right", "middle", "left"]):
			print("Invalide Tool")
			exit()
		global tools
		global toolsMoving
		try:
			if toolsMoving[str]:
				tools[str].setTargetPosition(90)
				tools[str].setEngaged(False)
				toolsMoving[str] = False
		except PhidgetException as ex:
			print(f"PhidgetException in tool_off for {str}: {ex.code} ({ex.description})")



except Exception as ex:
	print(f"An exception occurred: {ex}")		