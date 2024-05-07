import socket
import sys
from websocket import create_connection
import json
import gzip
import datetime

def main(args):   



    #save locally the geogridadata 

    UDP_IP = "127.0.0.1"
    UDP_PORT = 15800
    exit = False

    listen = "{\"type\":\"LISTEN\",\"content\":{\"gridId\":\"volpe_rd\"}}"
    ws = create_connection("wss://cityio.media.mit.edu:443/cityio/interface")
    ws.send(listen)

    sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP
    sock.bind((UDP_IP, UDP_PORT))

    sock.settimeout(0.6)

    #creating default loading files:
    result_geo = ws.recv()
    try:
        datajson = json.loads(result_geo)  # Convert JSON string to Python dictionary
    except json.JSONDecodeError:
        print("Received data is not in JSON format.")
        data = None

    #generate aruco codes link to the land-use type of each block:
    # Dictionary to hold the simplified data
    geogrid_types = datajson['content']['GEOGRID']['properties']['types']
    simplified_data = {}

    #aruco codes to associate with the name titles
    aruco_ids = [35, 38, 34, 17, 7, 43, 6, 4, 8, 10]

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
            print("Data has been saved to 'aruco_output.json'.")

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
            print("Data has been saved to 'geogriddata.json'.")

    geogrid_properties = datajson['content']['GEOGRID']['properties']
    if geogrid_properties is not None:
    # Write data to a file in JSON format with indentation
        with open('geogrid_properties.json', 'w') as file:
            json.dump(geogrid_properties, file, indent=4)
            print("Data has been saved to geogrid_properties.json")

    while not exit:
        now = datetime.datetime.now()
        timestamp = now.strftime("%Y%m%d-%H%M%S")
        try:
            data, addr = sock.recvfrom(1024*1000) # buffer size is 1024 bytes 24074
            #print("received message: %s" % data)

            # Decode the byte string to a regular string
            decoded_string = data.decode('utf-8')

            # Optional: Load the string into Python to manipulate or validate it as JSON
            # This step is not strictly necessary if you trust the data format
            try:
                json_data = json.loads(decoded_string)
                # This confirms it's valid JSON and you can manipulate it as a Python object if needed

            except json.JSONDecodeError:
                print(timestamp+": The decoded string is not valid JSON.")

                 # Create a filename with the timestamp
                filename = f"output-{timestamp}.txt"

                with open(filename, 'w') as file:
                    file.write(decoded_string)

                # print(json_data)
                json_data = None  # Or handle the error as needed

            if json_data is not None:
                # Convert the JSON data back to a JSON string for writing to file
                #data_grid_updated = json.dumps(json_data)
                #print(data_grid_updated)

                try:
                   ## data_grid_updated = json_data.replace('\\"', '"')

                    udpated_grid = {"type":"UPDATE_GRID","content":{"geogriddata":json_data}}
                    #udpated_grid = json.load(udpated_grid)
                    udpated_grid = json.dumps(udpated_grid)
                    
                    with open('test_data.json', 'w') as file:
                        file.write(udpated_grid)
                    
                    ws.send(listen)
                    ws.send(udpated_grid)
                    print(timestamp+ ": sent updated grid data")
                except ConnectionAbortedError:
                    print(timestamp+ ": Error connection with wss connecting again...")
                    ws = create_connection("wss://cityio.media.mit.edu:443/cityio/interface")
                    ws.send(listen)
                    udpated_grid = {"type":"UPDATE_GRID","content":{"geogriddata":json_data}}
                    # Writing the JSON string to a file
                    with open('test_data.json', 'w') as file:
                        file.write(json.dumps(udpated_grid, indent=4))
                    
                    ws.send(udpated_grid)
                
                print(f"Data was successfully written to data")
            else:
                pass
                #print("No data to write to the file.")
            ##print(json_string)


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
        finally:
            sock.close()
            print("Socket closed.")

    print("exit main loop")
    return

if __name__=="__main__":
    main(sys.argv[1:0])