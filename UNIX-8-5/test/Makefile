
server:server.o mmsql.o
	gcc server.o  mmsql.o -o server -lmysqlclient -lpthread
client:client.o
	gcc client.o -o client
%.o:%.c
	gcc -c $^ -o $@
clean:
	rm -rf client server client.o server.o mmsql.o
