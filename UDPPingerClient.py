from datetime import *
from socket import *

# Create a UDP socket
client_socket = socket(AF_INET, SOCK_DGRAM)
# general data about the RTT time and packet lost
min_RTT = 1
max_RTT = avg_RTT = lost_counter = 0
for i in range(10):
    flag = True
    start_time = datetime.now()
    msg = "Ping " + str(i) + " " + str(datetime.now().strftime("%H:%M:%S"))
    client_socket.sendto(msg.encode(), ("127.0.0.1", 12000))
    # setting timeout for receiving message from server
    client_socket.settimeout(1)
    try:
        data, addr = client_socket.recvfrom(12000)
        end_time = datetime.now()
        time_diff = ((end_time - start_time) * 1000).total_seconds()
    # if timeout is over
    except timeout as e:
        print("Request timed out")
        flag = False
        lost_counter += 1
    # if message from server received in time
    if flag:
        avg_RTT += time_diff
        if time_diff < min_RTT:
            min_RTT = time_diff
        if time_diff > max_RTT:
            max_RTT = time_diff
        print("RTT: ", time_diff, " second(s)")
        print("Message from server: ", data.decode())  # decode added for bytestream (ignores 'b')
    print()

# Printing the general data
print("Minimum time for RTT: ", min_RTT)
print("Maximum time for RTT: ", max_RTT)
print("Average time for RTT: ", avg_RTT/(10 - lost_counter))
print("Total packet lost: ", lost_counter/10*100, "%")

client_socket.close()
