import wave, struct
import socket
import soundfile as sf
import numpy as np

UDP_IP = "192.168.0.103"
#UDP_IP2 = "192.168.0.106"
UDP_PORT = 50005
#UDP_PORT2 = 54307
MESSAGE = "Hello World!"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE
Ns = 14088
Ms = 0
Nsl = 14000;

data, samplerate = sf.read('C:/Users/ALEX/Documents/test_song.wav') 
#print data[100000:100001]
data0 = data[:,0]
#print(len(data0))
#print data0[100000]
newdata = data0[0::10]
#print(len(newdata))
#print newdata[10000:10005]
newdata = 32797*newdata
#print newdata[10000:10005]
newdata = np.cast[np.uint16](newdata)
#print newdata[10000:10005]

trydata = newdata[14000:14001]
print trydata

sock = socket.socket(socket.AF_INET, # Internet
                    socket.SOCK_DGRAM) # UDP
sock.connect((UDP_IP, UDP_PORT))
sock.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))


#while True:
#    for i in range(len(newdata)):
        #sock = socket.socket(socket.AF_INET, # Internet
        #                     socket.SOCK_DGRAM) # UDP
        #sock.connect((UDP_IP, UDP_PORT))
        #sock.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))
        #print newdata[Nsl:Ns]
#        Nsl = Nsl + 48
#        Ns = Ns + 48
#        Ms = Ms + 1
        
#socks = socket.socket(socket.AF_INET, # Internet
#                    socket.SOCK_STREAM) # UDP
#socks.connect((UDP_IP2, UDP_PORT2))
#socks.sendto(MESSAGE, (UDP_IP2, UDP_PORT2))
