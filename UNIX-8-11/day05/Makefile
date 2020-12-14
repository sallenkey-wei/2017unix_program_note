server:server.o
	gcc server.o -o server -lpthread
server.o:server.c
	gcc -c server.c -o server.o
client:client.o
	gcc client.o -o client -lpthread
client.o:client.c
	gcc -c client.c -o client.o
clean:
	rm -rf client client.o server server.o
