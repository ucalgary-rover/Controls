import threading
import time
import websocket
import json
import curses

# Initialize variables
integers = [0]  # Example with a single integer for simplicity
int_tuples = (0, 0)
float_tuples = (0.0, 0.0)
lock = threading.Lock()
is_running = True

def clamp(value, min_value, max_value):
    return max(min(value, max_value), min_value)

def adjust_element(key):
    '''
        Here is where to adjust the keybindings for manipulation.
    
    '''

    global integers, int_tuples, float_tuples

    if key == 'q':
        integers[0] = clamp(integers[0] + 1, 0, 99)
    elif key == 'w':
        integers[0] = clamp(integers[0] - 1, 0, 99)

    elif key == 'e':
        int_tuples = (clamp(int_tuples[0] + 1, -99, 99), int_tuples[1])
    elif key == 'r':
        int_tuples = (clamp(int_tuples[0] - 1, -99, 99), int_tuples[1])
    elif key == 't':
        int_tuples = (int_tuples[0], clamp(int_tuples[1] + 1, -99, 99))
    elif key == 'y':
        int_tuples = (int_tuples[0], clamp(int_tuples[1] - 1, -99, 99))

    elif key == 'u':
        float_tuples = (clamp(float_tuples[0] + 0.1, -99.0, 99.0), float_tuples[1])
    elif key == 'i':
        float_tuples = (clamp(float_tuples[0] - 0.1, -99.0, 99.0), float_tuples[1])
    elif key == 'o':
        float_tuples = (float_tuples[0], clamp(float_tuples[1] + 0.1, -99.0, 99.0))
    elif key == 'p':
        float_tuples = (float_tuples[0], clamp(float_tuples[1] - 0.1, -99.0, 99.0))

def send_data(ws):
    while is_running:
        try:
            with lock:
                data = {
                    "integers": integers,
                    "int_tuples": [list(int_tuples)],  
                    "float_tuples": [list(float_tuples)], 
                }
            ws.send(json.dumps(data))
        except websocket.WebSocketConnectionClosedException:
            print("WebSocket connection closed. Attempting to reconnect...")
            time.sleep(5)
            start_websocket()  # Attempt to reconnect
            break
        except Exception as e:
            print(f"Error in sending data: {e}")
            break
        time.sleep(0.1)

def start_websocket():
    uri = "ws://10.13.176.189:6969" # Adjust IP address here to match your private ip address
    ws = websocket.WebSocketApp(uri, on_open=lambda ws: threading.Thread(target=send_data, args=(ws,)).start())
    ws.run_forever()

def display_values(stdscr):
    '''
        Display the values of the elements on the terminal.
        adjust the display to match the elements you are working with.
    '''

    stdscr.clear()
    stdscr.addstr("Control individual elements with specific keys. Press 'ESC' to exit.\n")
    stdscr.addstr(f"Integers: {integers} ('q' to increase, 'w' to decrease)\n")
    stdscr.addstr(f"Int Tuples: {int_tuples} ('e', 'r' to increase, 't', 'y' to decrease)\n")
    stdscr.addstr(f"Float Tuples: {float_tuples} ('u', 'i' to increase, 'o', 'p' to decrease)\n")
    stdscr.refresh()

def process_input(stdscr, key):
    adjust_element(key)
    display_values(stdscr)

def update_values(stdscr):
    global is_running
    stdscr.nodelay(True)
    display_values(stdscr)

    while is_running:
        try:
            key = stdscr.getkey()
            if key == '\x1b':  # ESC key for exit
                is_running = False
            else:
                process_input(stdscr, key)
        except Exception as e:
            pass

threading.Thread(target=start_websocket, daemon=True).start()
curses.wrapper(update_values)

is_running = False
print("Exiting application...")