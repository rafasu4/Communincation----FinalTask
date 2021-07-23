import time
import sys
from socket import *

# Create a UDP socket
# Notice the use of SOCK_DGRAM for UDP packets
try:
    serverSocket = socket(AF_INET, SOCK_DGRAM)
except socket.error:
    sys.exit("Failed at creating socket!")

# Assign IP address and port number to socket
serverSocket.bind(('127.0.0.1', 12000))

heartbeat_counter = 1

while True:
    # Receive the client packet along with the address it is coming from
    message, address = serverSocket.recvfrom(1024)
    # Capitalize the message from the client
    message = message.upper().decode()
    msg_arr = str(message).split(' ')
    received_msg = msg_arr[0]
    received_seq = msg_arr[1]
    received_time = msg_arr[2]
    elapsed_time = time.time() - float(received_time)

    # if timed elapsed is more than 5 seconds - client application assumed to be stopped
    if elapsed_time > 5:
        sys.exit('Client application has stopped.')

    # if timed elapsed is between 2 and 5 seconds - heartbeat assumed to be lost
    elif elapsed_time > 2:
        message = 'Heartbeat ' + str(heartbeat_counter) + ' is missing'
        print(message)
        serverSocket.sendto(message.encode(), address)

    else:
        msg = 'Heartbeat ' + received_seq + ' is received'
        print(msg)
        serverSocket.sendto(msg.encode(), address)

    heartbeat_counter += 1
