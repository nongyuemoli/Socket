src = $(wildcard *.cpp)
obj = $(patsubst %.cpp, %.o, $(src))

all: server client

server: server.o
	g++ server.o -o -g server -Wall -lpthread
client: client.o
	g++ client.o -o -g client -Wall

%.o:%.cpp
	gcc -c $< -Wall

.PHONY: clean all 
clean:
	-rm -rf server client $(obj)
