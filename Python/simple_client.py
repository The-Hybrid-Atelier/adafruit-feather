#!/usr/bin/env python3
# USAGE: (Turn OFF LED) python3 simple_client.py -ip 192.168.1.251 -c O 
# USAGE: (Turn ON LED) python3 simple_client.py -ip 192.168.1.251 -c L 

import socket
import argparse

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-ip', help='IP Address of Feather')
parser.add_argument('-p', help='Port of Feather Comm')
parser.add_argument('-c', help='Command to send to Feather')
args = parser.parse_args()

command = args.c + "\n"
HOST = args.ip
# HOST = '192.168.1.251'  # Cesar Home Network# The server's hostname or IP address
# HOST = '192.168.1.?'    # The Hybrid Atelier Network
PORT = int(args.p)       # The port used by the server: Telnet (23)

print("Sending")
print(command.encode())
print("\n")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(command.encode())
    data = s.recv(1024)

print('Received', repr(data))