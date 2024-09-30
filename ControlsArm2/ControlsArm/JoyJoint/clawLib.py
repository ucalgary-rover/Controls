from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
from Phidget22.Devices.DCMotor import *
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
	VHubSerial_motors = 697178
	VHubSerial_servo = 697066

	smoothing = 0.005

	claw = RCServo()
	tool_right = RCServo()
	tool_middle = RCServo()
	tool_left = RCServo()
	brush1 = DCMotor()
	brush2 = DCMotor()
	brush3 = DCMotor()

	brush_tools = [brush1, brush2, brush3]
	tools = {"tool_right" : tool_right, "tool_middle" : tool_middle, "tool_left" : tool_left}
	toolsMoving = {"tool_right" : False, "tool_middle" : False, "tool_left" : False}
	DCMotorInfo = [1.68, 1.68, 100, 1, 10, 10]

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
			print(f"\nInitializing Claw...\n\n") 	
		
			claw.setChannel(0)
			claw.setHubPort(0)
			claw.setDeviceSerialNumber(VHubSerial_servo)
			claw.openWaitForAttachment(1000)
			claw.setVoltage(RCServoVoltage.RCSERVO_VOLTAGE_7_4V)
			claw.setMinPulseWidth(500)
			claw.setMaxPulseWidth(2500)		

			print(f"\nSuccessfully initialized the claw!\n")	


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


	def turn_claw():
		claw.setTargetPosition((claw.getTargetPosition() + 180) % 360)
		claw.setEngaged(True) 
		print(f"ROVER: camera going to {claw.getTargetPosition()} degrees")
		
	def cam_left():
		global isClawMoving
		if isClawMoving:
			isClawMoving = False
			claw_off()
		else:
			isClawMoving = True
			claw_open()
			
	def cam_right():
		global isClawMoving
		if isClawMoving:
			isClawMoving = False
			claw_off()
		else:
			isClawMoving = True
			claw_close()

	def tool_init():
		global tools
		
		claw_init()
		claw.setTargetPosition(0)
		print(f"ROVER: camera going to {claw.getTargetPosition()} degrees")
		# setup tool servo
		try:
			i=1
			for tool in tools.values():
		    # Functions to initialize components 
				print(f"\nInitializing tool{i}...\n\n")
				tool.setChannel(i*2)
				tool.setHubPort(0)
				tool.setDeviceSerialNumber(VHubSerial_servo)
				tool.openWaitForAttachment(1000)
				tool.setVoltage(RCServoVoltage.RCSERVO_VOLTAGE_7_4V)
				tool.setMinPulseWidth(500)
				tool.setMaxPulseWidth(2500)
				i+=1		

				print(f"\nSuccessfully initialized tool {i}!\n")	

		except PhidgetException as ex: 
			traceback.print_exc() 
			print()
			print("PhidgetException " + str(ex.code) + " (" + ex.description + "): " + ex.details)
			print(f"Successfully quit program.\n\nGoodbye!\n")

		i = 1
		for brush in brush_tools:
			# setup tool brush
			brush.setDeviceSerialNumber(VHubSerial_motors)	
			# double check ports
			brush.setHubPort(i)
			i += 1
			try:
				brush.openWaitForAttachment(5000)
				print("Test connection")
				print("Drivers connected")
				print(f"Is brush {i} attached: ", brush.getAttached())
			except:
				print("Driver Failed to connect")
				print("Drivers not connected")
			else:
				brush.setCurrentLimit(10)
				brush.setTargetVelocity(0)
				brush.setAcceleration(5)
				brush.setAcceleration(100)
				brush.setTargetVelocity(0)
				brush.setFanMode(0x2) # 0x2 is FAN_MODE_ON

	def tool_lower(toolName):
		if (toolName not in ["tool_right", "tool_middle", "tool_left"]):
			print("Invalide Tool")
			exit()
		global tools
		global toolsMoving
		try:
			toolsMoving[toolName] = True
			tools[toolName].setTargetPosition(speeds[0]) 
			tools[toolName].setEngaged(True)
		except PhidgetException as ex:
			print(f"PhidgetException in tool_lower for {toolName}: {ex.code} ({ex.description})")

	def tool_raise(toolName):
		if (toolName not in ["tool_right", "tool_middle", "tool_left"]):
			print("Invalide Tool")
			exit()
		global tools
		global toolsMoving
		try: 
			toolsMoving[toolName] = True
			tools[toolName].setTargetPosition(speeds[1]) 
			tools[toolName].setEngaged(True) 
		except PhidgetException as ex:
			print(f"PhidgetException in tool_raise for {toolName}: {ex.code} ({ex.description})")

	def tool_off(toolName):
		if (toolName not in ["tool_right", "tool_middle", "tool_left"]):
			print("Invalide Tool")
			exit()
		global tools
		global toolsMoving
		try:
			if toolsMoving[toolName]:
				tools[toolName].setTargetPosition(90)
				tools[toolName].setEngaged(False)
				toolsMoving[toolName] = False
		except PhidgetException as ex:
			print(f"PhidgetException in tool_off for {toolName}: {ex.code} ({ex.description})")

	def brush_switch(index):
		if brush_tools[index].getIsMoving():
			brush_tools[index].setVelocityLimit(0)
		else:
			brush_tools[index].setVelocityLimit(1)



except Exception as ex:
	print(f"An exception occurred: {ex}")		
