#!/usr/bin/python

import socket

def main():
    
    txSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

    txSocket.sendto(str.encode("encoding"),("127.0.0.1", 8000))

    return

if __name__=="__main__":
    main()
