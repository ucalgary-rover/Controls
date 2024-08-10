#Created by the one and only Ramisa Islam

import pygame
from Phidget22.Phidget import *
from Phidget22.PhidgetException import *
from Phidget22.Devices.Stepper import *
from Phidget22.Devices.RCServo import *
from Phidget22.Devices.DCMotor import * 
import traceback
import time


VHubSerial_motors = 697178
VHubSerial_servo = 697066 #verify this matches what is in the rover

leftSideDriveMotors = DCMotor()
rightSideDriveMotors = DCMotor()

def driver_init():
    leftSideDriveMotors.setDeviceSerialNumber(VHubSerial_motors)
    rightSideDriveMotors.setDeviceSerialNumber(VHubSerial_motors)

    leftSideDriveMotors.setHubPort(0)
    rightSideDriveMotors.setHubPort(5)
    
    try:
        leftSideDriveMotors.openWaitForAttachment(5000)
        rightSideDriveMotors.openWaitForAttachment(5000)
        print("Test connection")
        print("Drivers connected")
        print("is left drive attached: ", leftSideDriveMotors.getAttached())
    except:
        print("Driver Failed to connect")
        print("Drivers not connected")
        
    else:
        leftSideDriveMotors.setCurrentLimit(10)
        leftSideDriveMotors.setTargetVelocity(0)
        leftSideDriveMotors.setAcceleration(5)
        #leftSideDriveMotors.setFanMode(DCMotorFanMode.DCMOTOR_FAN_MODE_OFF)

        rightSideDriveMotors.setCurrentLimit(10)
        rightSideDriveMotors.setTargetVelocity(0)
        rightSideDriveMotors.setAcceleration(5)
        #rightSideDriveMotors.setFanMode(DCMotorFanMode.DCMOTOR_FAN_MODE_OFF)

        #leftSideDriveMotors.setControlMode(DCMotorControlMode.DCMOTOR_CONTROL_MODE_SPEED)
        #rightSideDriveMotors.setControlMode(DCMotorControlMode.DCMOTOR_CONTROL_MODE_SPEED)
        leftSideDriveMotors.setAcceleration(100)
        rightSideDriveMotors.setAcceleration(100)
        # leftSideDriveMotors.setVelocityLimit(100)
        # rightSideDriveMotors.setVelocityLimit(100)
        leftSideDriveMotors.setTargetVelocity(0)
        rightSideDriveMotors.setTargetVelocity(0)
        leftSideDriveMotors.setFanMode(0x2)  # 0x2 is FAN_MODE_ON
        rightSideDriveMotors.setFanMode(0x2)  # 0x2 is FAN_MODE_ON

def drive_forward(speed):
    leftSideDriveMotors.setTargetVelocity(speed)
    rightSideDriveMotors.setTargetVelocity(-speed)

def drive_backward(speed):
    leftSideDriveMotors.setTargetVelocity(-speed)
    rightSideDriveMotors.setTargetVelocity(speed)

def drive_right(speed):
    leftSideDriveMotors.setTargetVelocity(speed)
    rightSideDriveMotors.setTargetVelocity(speed)

def drive_left(speed):
    leftSideDriveMotors.setTargetVelocity(-speed)
    rightSideDriveMotors.setTargetVelocity(-speed)

def drive_stop():
    leftSideDriveMotors.setTargetVelocity(0)
    rightSideDriveMotors.setTargetVelocity(0)
