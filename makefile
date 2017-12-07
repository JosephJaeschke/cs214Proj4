all:
	gcc -o server sorter_server.c -lpthread
	gcc -o client sorter_client.c -lpthread
server:
	gcc -o server sorter_server.c -lpthread
client:
	gcc -o client sorter_client.c -lpthread
clean:
	rm server
	rm client
	
