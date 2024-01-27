from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
from Phidget22.Devices.DCMotor import *
import time
from Phidget22.Devices.VoltageInput import *

# initalizing everything
VHubSerial_motors = 697103
VHubSerial_servo = 697066

leftSideDriveMotors = DCMotor()
rightSideDriveMotors = DCMotor()
leftWinchMotor = DCMotor()
bristleMotor = DCMotor()

ch_lsdm = VoltageInput()
ch_rsdm = VoltageInput()
ch_lwm = VoltageInput()
ch_bm = VoltageInput()

ch_lsdm.setDeviceSerialNumber(VHubSerial_motors)
ch_rsdm.setDeviceSerialNumber(VHubSerial_motors)
ch_lwm.setDeviceSerialNumber(VHubSerial_motors)
ch_bm.setDeviceSerialNumber(VHubSerial_motors)

ch_lsdm.setHubPort(0)
ch_rsdm.setHubPort(5)
ch_lwm.setHubPort(1)
ch_bm.setHubPort(2)
ch_bm.getChannelSubclass()
motors = [leftSideDriveMotors,rightSideDriveMotors,leftWinchMotor,bristleMotor]
motorsch = [ch_lsdm, ch_rsdm, ch_lwm, ch_bm]

# steppers
baseMotor = Stepper()
shoulderMotor = Stepper()
elbowMotor = Stepper()
wristMotor = Stepper()
wrist1Motor = Stepper()
wrist2Motor = Stepper()

ch_S_bm = VoltageInput()
ch_S_sm = VoltageInput()
ch_S_em = VoltageInput()
ch_S_wm0 = VoltageInput()
ch_S_wm1 = VoltageInput()
ch_S_wm2 = VoltageInput()

ch_S_bm.setDeviceSerialNumber(VHubSerial_servo)
ch_S_sm.setDeviceSerialNumber(VHubSerial_servo)
ch_S_em.setDeviceSerialNumber(VHubSerial_servo)
ch_S_wm0.setDeviceSerialNumber(VHubSerial_servo)
ch_S_wm1.setDeviceSerialNumber(VHubSerial_servo)
ch_S_wm2.setDeviceSerialNumber(VHubSerial_servo)

ch_S_bm.setHubPort(0)
ch_S_sm.setHubPort(1)
ch_S_em.setHubPort(2)
ch_S_wm0.setHubPort(3)
ch_S_wm1.setHubPort(4)
ch_S_wm2.setHubPort(5)
motors_S = [baseMotor,shoulderMotor,elbowMotor,wristMotor, wrist1Motor, wrist2Motor]
motorsch_S = [ch_S_bm, ch_S_sm, ch_S_em, ch_S_wm0, ch_S_wm1, ch_S_wm2]