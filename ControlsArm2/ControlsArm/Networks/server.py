import asyncio
import websockets
import json
import serial
import gnss

gnss_port = 'com20'         # Serial port the gnss is using, found using Device Manager on windows
gnss_period = 0.5           # Period between polls to the GNSS
server_ip = "10.13.68.10"   # IP address of the machine running this program, found using ipconfig on windows


server_port = 8765

# Setup serial port and generator function
serialPort = serial.Serial(port = gnss_port, baudrate=38400, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
gen = gnss.read_gnss_data(serialPort, gnss_period)

async def send_gnss_data(websocket, path):
    """
    Sends GNSS data to client using websocket

    Args:
        websocket: IP address of the machine running this program, found using ipconfig on windows
        path: Not fully sure if we need this, ChatGPT used it as the port but it doesn't explicitly get used
    """
    while True:
        # Get the current data from the GNSS
        current_position = next(gen)

        # Convert data to JSON format
        data_json = json.dumps({"lat": current_position[0], "lon": current_position[1]})

        # Send the JSON data to the client
        await websocket.send(data_json)
        print("Sent to client.")

        # Wait for one second before sending the next data
        await asyncio.sleep(1)

# Code to facilitate the server
async def main():
    async with websockets.serve(send_gnss_data, server_ip, server_port):
        await asyncio.Future()  # Run forever

asyncio.run(main())