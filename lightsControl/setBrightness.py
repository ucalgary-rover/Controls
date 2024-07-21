# coded by sajan and chatgpt
import serial
import time

# Open serial connection to Arduino
# make sure to have same baud rate and "COM" port as the arduino file (applyBrightness.ino)
arduino = serial.Serial('COM3', 9600) 
time.sleep(2)  # Allow time for serial connection to establish

# input options that will be accepted
# 0 -> lights off
# 1 -> 25% bright
# 2 -> 50% bright
# 3 -> 100% bright
options = [0,1,2,3]

# will record input and send to ardiono
def send_brightness():
    while True:
        # relays the options to the user, CAN BE DELETED CUZ LOWKEY UGLY TO LOOK AT
        print("\nSelect brightness level for headlights:\n0: 0% (Off)\n1: 25% brightness\n2: 50% brightness\n3: 100% brightness\n")
        try:
            brightness_level = int(input("Enter the number corresponding to the brightness level (0-3): "))
            # if the user input NOT in the options error message will pop up
            if brightness_level not in options:
                raise ValueError("\n\nare you dumb ENTER 0-3\n\n")
            
            # Send the brightness level to the Arduino as bytes
            arduino.write(bytes([brightness_level]))
            
            # idrk what this does but i think it prints the response from the arduino
            confirmation = arduino.readline().decode().strip()
            print("\nArduino Response:", confirmation)
        
        # some more error handling
        except ValueError as e:
            print(e)
        except Exception as e:
            print("Error:", e)

if __name__ == "__main__":
    send_brightness()
