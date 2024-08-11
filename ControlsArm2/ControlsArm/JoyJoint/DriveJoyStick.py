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

leftSideFrontDriveMotors = DCMotor()
rightSideFrontDriveMotors = DCMotor()

leftSideBackDriveMotors = DCMotor()
rightSideBackDriveMotors = DCMotor()

def driver_init():
    leftSideFrontDriveMotors.setDeviceSerialNumber(VHubSerial_motors)
    rightSideFrontDriveMotors.setDeviceSerialNumber(VHubSerial_motors)
    leftSideBackDriveMotors.setDeviceSerialNumber(VHubSerial_motors)
    rightSideBackDriveMotors.setDeviceSerialNumber(VHubSerial_motors)

    leftSideFrontDriveMotors.setHubPort(0)
    rightSideFrontDriveMotors.setHubPort(5)
    # double check following hub ports
    leftSideBackDriveMotors.setHubPort(1)
    rightSideBackDriveMotors.setHubPort(4)
    
    try:
        leftSideFrontDriveMotors.openWaitForAttachment(5000)
        rightSideFrontDriveMotors.openWaitForAttachment(5000)
        leftSideBackDriveMotors.openWaitForAttachment(5000)
        rightSideBackDriveMotors.openWaitForAttachment(5000)
        print("Test connection")
        print("Drivers connected")
        print("is left front drive attached: ", leftSideFrontDriveMotors.getAttached())
        print("is right front drive attached: ", rightSideFrontDriveMotors.getAttached())
        print("is left back drive attached ", leftSideBackDriveMotors.getAttached())
        print("is right back drive attached: ", rightSideBackDriveMotors.getAttached())
    except:
        print("Driver Failed to connect")
        print("Drivers not connected")
        
    else:
        leftSideFrontDriveMotors.setCurrentLimit(10)
        leftSideFrontDriveMotors.setTargetVelocity(0)
        leftSideFrontDriveMotors.setAcceleration(5)
        #leftSideFrontDriveMotors.setFanMode(DCMotorFanMode.DCMOTOR_FAN_MODE_OFF)
        rightSideFrontDriveMotors.setCurrentLimit(10)
        rightSideFrontDriveMotors.setTargetVelocity(0)
        rightSideFrontDriveMotors.setAcceleration(5)
        #rightSideFrontDriveMotors.setFanMode(DCMotorFanMode.DCMOTOR_FAN_MODE_OFF)
        
        leftSideBackDriveMotors.setCurrentLimit(10)
        leftSideBackDriveMotors.setTargetVelocity(0)
        leftSideBackDriveMotors.setAcceleration(5)
        #leftSideBackDriveMotors.setFanMode(DCMotorFanMode.DCMOTOR_FAN_MODE_OFF)
        rightSideBackDriveMotors.setCurrentLimit(10)
        rightSideBackDriveMotors.setTargetVelocity(0)
        rightSideBackDriveMotors.setAcceleration(5)
        #rightSideBackDriveMotors.setFanMode(DCMotorFanMode.DCMOTOR_FAN_MODE_OFF)

        #leftSideFrontDriveMotors.setControlMode(DCMotorControlMode.DCMOTOR_CONTROL_MODE_SPEED)
        #rightSideFrontDriveMotors.setControlMode(DCMotorControlMode.DCMOTOR_CONTROL_MODE_SPEED)
        leftSideFrontDriveMotors.setAcceleration(100)
        rightSideFrontDriveMotors.setAcceleration(100)
        #leftSideBackDriveMotors.setControlMode(DCMotorControlMode.DCMOTOR_CONTROL_MODE_SPEED)
        #rightSideBackDriveMotors.setControlMode(DCMotorControlMode.DCMOTOR_CONTROL_MODE_SPEED)
        leftSideBackDriveMotors.setAcceleration(100)
        rightSideBackDriveMotors.setAcceleration(100)
        
        # leftSideFrontDriveMotors.setVelocityLimit(100)
        # rightSideFrontDriveMotors.setVelocityLimit(100)
        # leftSideBackDriveMotors.setVelocityLimit(100)
        # rightSideBackDriveMotors.setVelocityLimit(100)
        
        leftSideFrontDriveMotors.setFanMode(0x2)  # 0x2 is FAN_MODE_ON
        rightSideFrontDriveMotors.setFanMode(0x2)  # 0x2 is FAN_MODE_ON
        leftSideBackDriveMotors.setFanMode(0x2)  # 0x2 is FAN_MODE_ON
        rightSideBackDriveMotors.setFanMode(0x2)  # 0x2 is FAN_MODE_ON

def drive_forward(speed):
    leftSideFrontDriveMotors.setTargetVelocity(speed)
    rightSideFrontDriveMotors.setTargetVelocity(-speed)
    leftSideBackDriveMotors.setTargetVelocity(speed)
    rightSideBackDriveMotors.setTargetVelocity(-speed)

def drive_backward(speed):
    leftSideFrontDriveMotors.setTargetVelocity(-speed)
    rightSideFrontDriveMotors.setTargetVelocity(speed)
    leftSideBackDriveMotors.setTargetVelocity(-speed)
    rightSideBackDriveMotors.setTargetVelocity(speed)

def drive_right(speed):
    leftSideFrontDriveMotors.setTargetVelocity(speed)
    rightSideFrontDriveMotors.setTargetVelocity(speed)
    leftSideBackDriveMotors.setTargetVelocity(speed)
    rightSideBackDriveMotors.setTargetVelocity(speed)

def drive_left(speed):
    leftSideFrontDriveMotors.setTargetVelocity(-speed)
    rightSideFrontDriveMotors.setTargetVelocity(-speed)
    leftSideBackDriveMotors.setTargetVelocity(-speed)
    rightSideBackDriveMotors.setTargetVelocity(-speed)

def drive_stop():
    leftSideFrontDriveMotors.setTargetVelocity(0)
    rightSideFrontDriveMotors.setTargetVelocity(0)
    leftSideBackDriveMotors.setTargetVelocity(0)
    rightSideBackDriveMotors.setTargetVelocity(0)
