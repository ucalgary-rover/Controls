import serial
from pyubx2 import UBXReader
import time

# Module: https://pypi.org/project/pyubx2/
# Documentation: https://github.com/semuconsulting/pyubx2

# University of Calgary approx coordinates
# Latitude: 51.0782° N
# Longitude: -114.1350° W

def read_gnss_data(serial_port, period, last_valid_position=None, debug_messages=False):
    """
    Polls GNSS for data. Processes, delays for the intended period, and error handles for getting latitide and longitude.
    This is agenerator function, meaning that after it is initialized, the next value can be called using
    next(generator_variable_name)

    Args:
        serial_port: Serial port that the gnss is using on the machine. On windows it can be found using the Device Manager
        period: The period of time between polls
        last_valid_position: Needed to prevent drop-out readings from sending bad data, initialized to None
        debug_messages: Can be used to print debug statements, initialized to False

    Yields:
        float tuple: A tuple of floats containing lattitide and longitude
    """

    ubr = UBXReader(serial_port)

    def delay_thread():
        while True:
            time.sleep(period)
            yield ()

    delay_gen = delay_thread()

    while True:
        (raw_data, parsed_data) = ubr.read()

        try:
            # Check if the data is valid
            if parsed_data.lat is not None and parsed_data.lon is not None:
                last_valid_position = (parsed_data.lat, parsed_data.lon)
                if(debug_messages): print(f'Current Position: Latitude {parsed_data.lat}, Longitude {parsed_data.lon}')
            else:
                if (debug_messages): print("Received bad data. Using last valid position.")

        except Exception as e:
            if (debug_messages): print(f"Error processing data: {e}. Using last valid position.")

        # Introduce the threaded delay
        next(delay_gen)

        # Yield last_valid_position if needed
        yield last_valid_position