#!/usr/bin/env python3
import argparse
import json
import asyncio
import websockets

parser = argparse.ArgumentParser()
parser.add_argument('-ip', help='IP Address of Feather')
parser.add_argument('-p', help='Port of Feather Comm')
parser.add_argument('-c', help='Command to send to Feather')
args = parser.parse_args()


HOST = args.ip
# HOST = '192.168.1.251'  # Cesar Home Network# The server's hostname or IP address
# HOST = '192.168.1.?'    # The Hybrid Atelier Network
PORT = int(args.p)       # The port used by the server: Telnet (23)

# WS client example
async def message_handling():
  uri = "ws://"+HOST+":"+str(PORT)
  async with websockets.connect(uri) as websocket:
    print("Client >> Socket opened\n")
    greeting = {"name": "python-client", "version": "1.0"}
    await websocket.send(json.dumps(greeting))
    while True:
      message = await websocket.recv()
    try:
      message = json.loads(message)
      print(f"Client < {message}")
      # TODO
      print("DOING SOMETHING WITH MESSAGE")
    except Exception as e:
      print("Invalid JSON")

  

asyncio.get_event_loop().run_until_complete(message_handling())
  
  
  


