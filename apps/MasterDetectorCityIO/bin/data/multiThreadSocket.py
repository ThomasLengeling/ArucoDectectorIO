import socket
import asyncio
from time import sleep
import websockets
import threading
import json
import datetime
import os

# Shared data structure for communication between threads
data_queue = []
table_name = "volpe_rd"

deploy_uri = "wss://cityio.media.mit.edu:443/cityio/interface"
wss_listen = "{\"type\":\"LISTEN\",\"content\":{\"gridId\":\""+table_name+"\"}}"

#aruco codes to associate with the name titles
aruco_ids = [35, 38, 34, 17, 7, 43, 6, 4, 8, 10]

def obtain_table():
    with open("config.json", 'r') as file:
        config_info = json.load(file)

    deploy_uri = config_info['deploy']['url']
    table_name = config_info['deploy']['tableName']
    wss_listen = "{\"type\":\"LISTEN\",\"content\":{\"gridId\":\""+table_name+"\"}}"

    print("URL: "+deploy_uri  )
    print("Table Name: "+table_name  )

async def process_json_files():
    async with websockets.connect(deploy_uri, max_size=2**24, compression="deflate") as websocket:
        await websocket.send(wss_listen)

        #creating default loading files:
        result_geo = await websocket.recv()
        print("Data recieved!: "+deploy_uri)

        try:
            datajson = json.loads(result_geo)  # Convert JSON string to Python dictionary
        except json.JSONDecodeError:
            print("Received data is not in JSON format.")
            data = None

        #generate aruco codes link to the land-use type of each block:
        # Dictionary to hold the simplified data
        geogrid_types = datajson['content']['GEOGRID']['properties']['types']
        simplified_data = {}

        # Extract only the 'name' and 'color' from each entry
        for [key, value], ids  in zip(geogrid_types.items(), aruco_ids):
            simplified_data[key] = {"name": key, "color": value["color"], "aruco_id": ids, "height":value['height']}

        # Convert the simplified dictionary back to a JSON string
        aruco_output = json.dumps(simplified_data, indent=4)
        aruco_dict = {"Codes": simplified_data} # Create a dictionary to represent the desired JSON structure

        if aruco_output is not None:
            # Write data to a file in JSON format with indentation
            with open('aruco_codes.json', 'w') as file:
                json.dump(aruco_dict, file, indent=4)
                print("Data has been saved to aruco_output.json")

        #features
        geogrid_features = datajson['content']['GEOGRID']['features']
        if geogrid_features is not None:
            # Write data to a file in JSON format with indentation
            with open('geogrid_features.json', 'w') as file:
                json.dump(geogrid_features, file, indent=4)
                print("Data has been saved to geogrid_features.json")

        #geogriddata
        geogriddata = datajson['content']['GEOGRIDDATA']
        if geogriddata is not None:
            # Write data to a file in JSON format with indentation
            with open('geogriddata.json', 'w') as file:
                json.dump(geogriddata, file, indent=4)
                print("Data has been saved to geogriddata.json")

        geogrid_properties = datajson['content']['GEOGRID']['properties']
        if geogrid_properties is not None:
        # Write data to a file in JSON format with indentation
            with open('geogrid_properties.json', 'w') as file:
                json.dump(geogrid_properties, file, indent=4)
                print("Data has been saved to geogrid_properties.json")

# Function to handle UDP communications
def udp_server():

    #default values
    udp_ip = '127.0.0.1' 
    udp_port = 15800

    with open("networkUDP.json", 'r') as file:
        udp_json = json.load(file)

    # Convert JSON string to Python object
    network_data = json.loads(udp_json)
    # Extract the network data
    udp_ip = network_data[1]["network_0"]["ip"]
    udp_port = network_data[1]["network_0"]["port"]

    exit = False

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((udp_ip, udp_port))

    print("UDP Server listening: "+udp_ip+ " "+udp_port)

    while not exit:

        now = datetime.datetime.now()
        timestamp = now.strftime("%Y%m%d-%H%M%S")
        try:
            data, addr = sock.recvfrom(1024*1000)  # buffer size is 1024 bytes
            #print("received message:", data.decode('utf-8'))
            # Append data to shared queue
            # Decode the byte string to a regular string
            decoded_string = data.decode('utf-8')

            try:
                json_data = json.loads(decoded_string)
                # This confirms it's valid JSON and you can manipulate it as a Python object if needed

            except json.JSONDecodeError:
                print(timestamp+": The decoded string is not valid JSON.")

                # Create a filename with the timestamp
                #filename = f"output-{timestamp}.txt"
                #with open(filename, 'w') as file:
                 #   file.write(decoded_string)

                # print(json_data)
                json_data = None  # Or handle the error as needed

            if json_data is not None:
                print(timestamp+": got new data")
                data_queue.insert(0, json_data)
            else:
                pass
            
        except socket.timeout:
            #print("Socket operation timed out")
            pass
        except OSError as e:
            print(f"Socket error occurred: {e}")
            ##print("error socket - waiting for input udp") 
            #If no data is received you end up here, but you can ignore
            #the error and continue
            pass
        except KeyboardInterrupt:
            exit = True
            print( "Received Ctrl+C... initiating exit")
            break
        ##finally:
            ##sock.close()
            ##Sprint("Socket closed.")


# Function to handle WebSocket client communications with reconnection
async def websocket_client():
    while True:
        try:
            async with websockets.connect(deploy_uri, max_size=2**23, compression="deflate") as websocket:
                print("Connected to WebSocket server at", deploy_uri)
                await websocket.send(wss_listen)
                while True:
                    
                    now = datetime.datetime.now()
                    timestamp = now.strftime("%Y%m%d-%H%M%S")
                    if data_queue:
                        print("sending new data")
                        json_data = data_queue.pop(0)

                        udpated_grid = json.dumps({"type":"UPDATE_GRID","content":{"geogriddata":json_data}},separators=(',', ':'))
                        
                        await websocket.send(udpated_grid)
                        print(timestamp+ ": sent updated grid data")
                    await asyncio.sleep(0.1)  # relax the loop
        except (websockets.exceptions.WebSocketException, socket.error) as e:
            print(f"WebSocket error: {e}, attempting to reconnect in 5 seconds...")
            await asyncio.sleep(0.5)  # wait before attempting to reconnect

def main():
    #get table and url for the wss
    obtain_table()

    #process the default data for the grid setup
    asyncio.get_event_loop().run_until_complete(process_json_files())
    
    #waiting for complete
    sleep(3)

    # Thread for running the UDP server
    udp_thread = threading.Thread(target=udp_server)
    udp_thread.daemon = True
    udp_thread.start()

    try:
        # Start the WebSocket client in the main asyncio event loop
        asyncio.run(websocket_client())
    except KeyboardInterrupt:
        print("KeyboardInterrupt caught, stopping servers...")
        udp_thread.join()  # Wait for the UDP thread to finish
        print("Servers have been stopped.")

if __name__ == "__main__":
    main()