# multicast-with-apr
lan udp multicast with apache portable runtime.

Usage:  

1, run as sending side, 192.168.0.100 is the ip of the network interface which to send multicast packets  

	./app 192.168.0.100 s

2, run as recving side, only 0.0.0.0 works for linux  

	./app 0.0.0.0 c
