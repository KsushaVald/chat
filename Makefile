client: window.o client.o
		gcc -o  client  client.o window.o -lncurses -lpthread -w 
client.o: client.c libwindow.h
		gcc -c client.c -w
window.o: window.c libwindow.h
		gcc -c window.c
