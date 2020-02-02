src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

all: epoll

epoll: epoll.o
	gcc epoll.o -o epoll -Wall -g

%.o:%.c
	gcc -c $< -Wall

.PHONY: clean all
clean:
	-rm -rf epoll $(obj)
