all:
	gcc -o sorter_server sorter_server.c -lpthread
	gcc -o sorter_client sorter_client.c -lpthread
sorter_server:
	gcc -o sorter_server sorter_server.c -lpthread
sorter_client:
	gcc -o sorter_client sorter_client.c -lpthread
clean:
	rm sorter_server
	rm sorter_client
	
