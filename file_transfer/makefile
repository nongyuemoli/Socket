src = $(wildcard *.cpp)
obj = $(patsubst %.cpp, %.o, $(src))

all:server client

server:server.o
	g++ -g -o server server.o
client:client.o
	g++ -g -o client client.o
server.o:server.cpp
	g++ -g -c server.cpp
client.o:client.cpp
	g++ -g -c client.cpp
.PHONY: clean all
clean:
	-rm -rf server client $(obj)
