server:
	gcc server.c socket.c init.c epoll.c mmsql.c linklist.c server_action.c pwdmgmt.c -o server -lpthread -lmysqlclient -std=gnu99 
client:
	gcc client.c socket.c init.c pwdmgmt.c menushow.c menumgmt.c linklist.c -o client -lpthread -std=gnu99 -D SOCK_FOR_CLIENT 
allow:
	gcc allow_addf_action.c pwdmgmt.c -o allow_addf -std=gnu99
clean:
	rm -rf client server
