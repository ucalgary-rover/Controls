import asyncio
import websockets
import json
import logging

logging.basicConfig(level=logging.INFO)

async def handler(websocket, path):
    logging.info("Client connected")
    try:
        async for message in websocket:
            data = json.loads(message)
            if all(key in data for key in ['integers', 'int_tuples', 'float_tuples']):
                processed_data = process_data(data)
                logging.info(f"Processed data: {processed_data}")
            else:
                logging.warning("Invalid data format received.")
    except Exception as e:
        logging.error(f"Error in WebSocket connection: {e}")
    finally:
        logging.info("Client disconnected")

def process_data(data):
    processed_data = {
        'integers': [],
        'int_tuples': [],
        'float_tuples': []
    }
    
    # Process integers
    integers = data.get('integers', [])
    if isinstance(integers, int):  # Single integer received
        processed_data['integers'].append(integers)
    elif isinstance(integers, list):  # List of integers received
        processed_data['integers'].extend([i for i in integers if isinstance(i, int)])
    
    # Helper to process tuple data
    def process_tuples(tuples, data_type='int'):
        processed_tuples = []
        if isinstance(tuples, tuple):  # Single tuple received
            tuples = [tuples]
        for item in tuples:
            if isinstance(item, (list, tuple)):
                if all(isinstance(x, data_type) for x in item):
                    processed_tuples.append(tuple(item))
        return processed_tuples

    # Process integer tuples
    int_tuples = data.get('int_tuples', [])
    processed_data['int_tuples'] = process_tuples(int_tuples, data_type=int)
    
    # Process float tuples
    float_tuples = data.get('float_tuples', [])
    processed_data['float_tuples'] = process_tuples(float_tuples, data_type=float)
    
    return processed_data


async def main_asyncio():
    async with websockets.serve(handler, "127.0.0.1", 6969):  # Adjust IP address if needed
        logging.info("WebSocket Server running")
        await asyncio.Future()  # Runs forever

def main():
    asyncio.run(main_asyncio())

if __name__ == '__main__':
    main()