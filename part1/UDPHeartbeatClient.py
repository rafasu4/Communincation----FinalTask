
import sys
import time
from socket import *
import random

# Create a UDP socket
try:
    client_socket = socket(AF_INET, SOCK_DGRAM)
except socket.error:
    sys.exit("Failed at creating socket!")

heartbeat_counter = 1

for i in range(10):
    rand = random.randint(0, 10)
    sent_time = time.time()
    msg = 'Heartbeat ' + str(i+1) + ' ' + str(sent_time)

    # Simulates packet loss
    if rand < 4:
        time.sleep(2)
        client_socket.sendto(msg.encode(), ("127.0.0.1", 12000))

    # Simulates silence from server
    elif rand == 10:
        time.sleep(5)
        client_socket.sendto(msg.encode(), ("127.0.0.1", 12000))
        print("Silence for 5 seconds - Server should assumed that client has shut down")
        sys.exit()

    # Packet sent successfully
    else:
        client_socket.sendto(msg.encode(), ("127.0.0.1", 12000))

    message, addr = client_socket.recvfrom(12000)
    message = message.upper()
    print("Message from server: ", message.decode())

client_socket.close()
