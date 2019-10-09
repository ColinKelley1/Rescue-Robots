all: hw5Client hw5Server
hw5Client: client.cpp SafeQcfg.o SafeQueue.o
	g++ -std=c++11 client.cpp SafeQcfg.o -lpthread -o hw5Client
hw5Server: server.cpp Log.o
	g++ -std=c++11 server.cpp Log.o -lpthread -o hw5Server
SafeQcfg.o: SafeQcfg.cpp
	g++ -std=c++11 -c SafeQcfg.cpp -o SafeQcfg.o
SafeQueue.o: SafeQueue.cpp SafeQueue.h
	g++ -std=c++11 -c SafeQueue.cpp -o SafeQueue.o
Log.o: Log.cpp Log.h
	g++ -std=c++11 -c Log.cpp -o Log.o

clean:
	rm *.o
	rm hw5Client
	rm hw5Server
