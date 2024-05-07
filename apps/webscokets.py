#!/usr/bin/python

import socket
from threading import Thread
from time import sleep
import sys

exit = False

def rxThread(portNum):
    global exit
    
    #Generate a UDP socket
    rxSocket = socket.socket(socket.AF_INET, #Internet
                             socket.SOCK_DGRAM) #UDP
                             
    #Bind to any available address on port *portNum*
    rxSocket.bind(("",portNum))
    
    #Prevent the socket from blocking until it receives all the data it wants
    #Note: Instead of blocking, it will throw a socket.error exception if it
    #doesn't get any data
    
    #rxSocket.setblocking(0)
    rxSocket.settimeout(.1)

    print("RX: Receiving data on UDP port " + str(portNum))
    print(" ")
    
    while not exit:
        try:
            #Attempt to receive up to 1024 bytes of data
            data,addr = rxSocket.recvfrom(1024) 
            #Echo the data back to the sender
            rxSocket.sendto(data,addr)

        except socket.error:
            #If no data is received, you get here, but it's not an error
            #Ignore and continue
            pass

        sleep(.1)
    
def txThread(portNum):
    global exit
    
    
def main(args):    
    global exit
    print("UDP Tx/Rx Example application")
    print("Press Ctrl+C to exit")
    print("")
    
    portNum = 8000
   
    udpRxThreadHandle = Thread(target=rxThread,args=(portNum,))    
    udpRxThreadHandle.start()
        
    sleep(.1)
    
    #Generate a transmit socket object
    txSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    
    #Do not block when looking for received data (see above note)
    txSocket.setblocking(0) 
   
    print( "Transmitting to 127.0.0.1 port " + str(portNum))
    print( "Type anything and press Enter to transmit")
    while True:
        try:

            #Attempt to receive the echo from the server
            data, addr = txSocket.recvfrom(1024)
            sleep(.2)
            
            print("RX: " + str(data) )

             #Retrieve input data 
            txChar = "sa" #input("TX: ")
            #print(f"You typed: {txChar}")
                        
            #Transmit data to the local server on the agreed-upon port
            txSocket.sendto(str.encode(txChar),("127.0.0.1", portNum))
            
            #Sleep to allow the other thread to process the data
     
            

        except socket.error:    
            #If no data is received you end up here, but you can ignore
            #the error and continue
            pass   
        except KeyboardInterrupt:
            exit = True
            print( "Received Ctrl+C... initiating exit")
            break
        sleep(.1)
         
    udpRxThreadHandle.join()
        
    return

if __name__=="__main__":
    main(sys.argv[1:0])
