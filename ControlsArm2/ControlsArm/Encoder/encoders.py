
from Phidget22.Phidget import *
from Phidget22.Devices.Encoder import *
import time

#class PositionChangeListener(Phidget22.Devices.Encoder.PositionChangeCallback):
def onPositionChange(self, positionChange, timeChange, indexTriggered):
	print(f"Position: {self.getPosition()}")

def main():
    try:
        # Create and configure the encoder object
        encoder0 = Encoder()
        encoder0.setDeviceSerialNumber(697066)
        encoder0.setHubPort(3)
        #position_change_listener = PositionChangeListener()
        #encoder.setOnPositionChangeHandler(position_change_listener.onPositionChange)
        encoder0.setOnPositionChangeHandler(onPositionChange)

        # Open and wait for attachment
        encoder0.openWaitForAttachment(5000)

        print("Encoder attached. Press Ctrl+C to exit.")
        
        # Keep the program running to continuously monitor position changes
        while True:
            time.sleep(1)

    except PhidgetException as ex:
        print(f"PhidgetException: {ex.code} ({ex.description})")

    except KeyboardInterrupt:
        print("Exiting...")

    finally:
        # Clean up
        encoder0.close()

if __name__ == "__main__":
    main()
