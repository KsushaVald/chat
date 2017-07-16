client: window.o client.o
		gcc -o  client  client.o window.o -lncurses -lpthread -w -g 
client.o: client.c libwindow.h
		gcc -c client.c -w -g
window.o: window.c libwindow.h
		gcc -c window.c -g
